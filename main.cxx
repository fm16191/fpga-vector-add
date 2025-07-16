#include <algorithm>
#include <chrono>
#include <cmath>
#include <charconv>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <string>
#include <sycl/sycl.hpp>
#include <vector>

#include "kernel.hpp"

#if FPGA_HARDWARE || FPGA_EMULATOR || FPGA_SIMULATOR
    #include <sycl/ext/intel/fpga_extensions.hpp>
#endif

using namespace sycl;
using std::cout;
using std::cerr;
using std::vector;
using std::string;
using std::chrono::steady_clock;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::microseconds;

static constexpr size_t NB_ITER = 15;
static constexpr double tolerance = 1e-10;

struct results_t {
    double sum, min, max;
    double mean, standard_deviation;
};

static results_t timers_stats(vector<double> const &timers)
{
    auto const [min, max] = std::minmax_element(timers.begin(), timers.end());
    auto const sum = std::accumulate(timers.begin(), timers.end(), 0.0);
    auto const mean = sum / double(timers.size());
    auto const sdev = sqrt(std::accumulate(timers.begin(), timers.end(), 0.0,
        [&](double acc, double const time) { return acc + (time - mean) * (time - mean); }) / double(timers.size()));

    return results_t{ sum, *min, *max, mean, sdev };
}

static results_t timers_print(vector<double> const &timers, string const name)
{
    results_t res = timers_stats(timers);
    cout << "-----------------------------------------------------------\n" << name << "\n";
    printf("Average execution time: (mean ± σ)   %.1f us ± %.1f µs\n", res.mean, res.standard_deviation);
    printf("                        (min … max)  %.1f us … %.1f µs\n", res.min, res.max);
    printf("\n");
    return res;
}

static inline double get_time_us(struct timespec start, struct timespec end)
{
    return static_cast<double>(end.tv_sec - start.tv_sec) * 1e6 +
           static_cast<double>(end.tv_nsec - start.tv_nsec) / 1e3;
}

int main(int argc, char *argv[])
{
    size_t N = 1e7;
    if (argc > 1) N = static_cast<size_t>(std::stoi(std::string(argv[1])));

    const size_t problem_size = N;
    const size_t alloc_size_bytes = sizeof(DATATYPE) * problem_size;

#if FPGA_EMULATOR
    auto selector = sycl::ext::intel::fpga_emulator_selector_v;
#elif FPGA_SIMULATOR
    auto selector = sycl::ext::intel::fpga_simulator_selector_v;
#elif FPGA_HARDWARE
    auto selector = sycl::ext::intel::fpga_selector_v;
#else
    auto selector = default_selector_v;
#endif

    printf("\nCreating device queue - loading FPGA design\n");
    auto t1 = high_resolution_clock::now();
    sycl::queue queue(selector);
    auto t2 = high_resolution_clock::now();
    std::chrono::duration<double, std::milli> t_queue = t2 - t1;
    fprintf(stderr, "Queue created in %.2fs \n\n", t_queue.count() / 1e3);

    auto device = queue.get_device();

    size_t max_block_size = device.get_info<info::device::max_work_group_size>();
    size_t max_EU_count = device.get_info<info::device::max_compute_units>();

    fprintf(stderr, "Running on %s\n", device.get_info<info::device::name>().c_str());
    fprintf(stderr, "Device Max Work Group Size is: %lu\n", max_block_size);
    fprintf(stderr, "Device Max EUCount is: %lu\n", max_EU_count);
    fprintf(stderr, "\n");

    constexpr size_t one_gb = size_t(1ULL<<30); // 1073741824
    size_t global_mem_b = device.get_info<info::device::global_mem_size>();
    double global_mem_gb = double(global_mem_b) / double(one_gb);
    const double needed_mem_gb = double(sizeof(double) * 3 * problem_size) / double(one_gb);
    fprintf(stderr, "Device Max mem size                 : %.2f GB\n", global_mem_gb);
    fprintf(stderr, "Estimated mem size                  : %.2f GB\n", needed_mem_gb);

    if (needed_mem_gb > global_mem_gb) {
        cerr << "Insufficient device memory.";
        cerr << "Max Memory : " << global_mem_gb << " Gb, Needed Memory : " << needed_mem_gb << " Gb\n";
        return 1;
    }

    // Allocations
    fprintf(stderr, "Allocation host arrays ...   ");
    vector<DATATYPE> h_A(problem_size);
    vector<DATATYPE> h_B(problem_size);
    vector<DATATYPE> h_res(problem_size);
    fprintf(stderr, "done\n");

    fprintf(stderr, "Allocation device arrays ... ");
    DATATYPE *d_A   = malloc_device<DATATYPE>(problem_size, queue);
    DATATYPE *d_B   = malloc_device<DATATYPE>(problem_size, queue);
    DATATYPE *d_res = malloc_device<DATATYPE>(problem_size, queue);
    fprintf(stderr, "done\n");

    // Init
    fprintf(stderr, "Init device arrays ...       ");
    for (size_t i = 0; i < problem_size; ++i) {
        const double sinn = std::abs(sin(double(i))) * (i % 10);
        h_A[i] = DATATYPE(sinn);
        h_B[i] = DATATYPE(2 * sinn);
    }
    fprintf(stderr, "done\n");

    // Timers allocations
    vector<double> timers_cpu_to_fpga;
    vector<double> timers_fpga_kernel;
    vector<double> timers_fpga_to_cpu;

    vector<double> timers_fpga_total_usage;

    struct timespec simu_t1, simu_t2;

    struct timespec fpga_usage1, fpga_usage2;
    struct timespec cpu_to_fpga1, cpu_to_fpga2;
    struct timespec fpga_compute1, fpga_compute2;
    struct timespec fpga_to_cpu1, fpga_to_cpu2;

    clock_gettime(CLOCK_MONOTONIC, &simu_t1);

    // -- Kernel
    for (size_t t = 0; t < NB_ITER; ++t) {
        fprintf(stderr, "Copy from Host to Device ... ");
        clock_gettime(CLOCK_MONOTONIC, &fpga_usage1);
        clock_gettime(CLOCK_MONOTONIC, &cpu_to_fpga1);
        queue.memcpy(d_A, h_A.data(), alloc_size_bytes).wait();
        queue.memcpy(d_B, h_B.data(), alloc_size_bytes).wait();
        clock_gettime(CLOCK_MONOTONIC, &cpu_to_fpga2);
        fprintf(stderr, "done\n");

        /* Computation */
        clock_gettime(CLOCK_MONOTONIC, &fpga_compute1);
        fprintf(stderr, "Iteration %lu starting ...", t);
        launcher(d_A, d_B, d_res, problem_size, queue);
        queue.wait();
        fprintf(stderr, "done\n");
        clock_gettime(CLOCK_MONOTONIC, &fpga_compute2);

        fprintf(stderr, "Copy from Device to Host ... ");
        clock_gettime(CLOCK_MONOTONIC, &fpga_to_cpu1);
        queue.memcpy(h_res.data(), d_res, alloc_size_bytes).wait();
        clock_gettime(CLOCK_MONOTONIC, &fpga_to_cpu2);
        clock_gettime(CLOCK_MONOTONIC, &fpga_usage2);
        fprintf(stderr, "done\n");

        timers_cpu_to_fpga.push_back(get_time_us(cpu_to_fpga1, cpu_to_fpga2));
        timers_fpga_to_cpu.push_back(get_time_us(fpga_to_cpu1, fpga_to_cpu2));
        timers_fpga_kernel.push_back(get_time_us(fpga_compute1, fpga_compute2));
        timers_fpga_total_usage.push_back(get_time_us(fpga_usage1, fpga_usage2));
    }
    // -- Kernel

    clock_gettime(CLOCK_MONOTONIC, &simu_t2);
    double t_simu = get_time_us(simu_t1, simu_t2);

    results_t res_cpu_to_fpga = timers_print(timers_cpu_to_fpga, "-- copy CPU to FPGA --");
    results_t res_kernel_compute = timers_print(timers_fpga_kernel, "-- FPGA Kernel compute time --");
    results_t res_fpga_to_cpu = timers_print(timers_fpga_to_cpu, "-- copy FPGA to CPU --");

    results_t res_total_compute = timers_print(timers_fpga_total_usage, "-- Total FPGA Usage --");

    double t_simu_sec = t_simu / 1e6;

    printf("Total execution time: %.3f s\n", t_simu_sec);

    // Verification
    vector<double> diff(problem_size);
    for(size_t i = 0; i < problem_size; ++i)
        diff[i] = std::abs(double(h_res[i]) - double(h_A[i] + h_B[i]));

    printf("\n");
    printf("items : %0.1e\n", double(problem_size));
    printf("datatype size : %ld\n", sizeof(DATATYPE));
    printf("tolerance : %.1e\n", tolerance);

    // Verification stats
    results_t res = timers_stats(diff);

    printf("-----------------------------------------------------------\n");
    printf("Average precision difference: (mean ± σ)   %.2e ± %.2e\n", res.mean, res.standard_deviation);
    printf("                              (min … max)  %.2e … %.2e\n", res.min, res.max);
    printf("\n");

    // Report II and bandwidth
    // Get frequency from executable file (using aocl info). If not found, assume it's 480MHz
    double frequency = 480e6;
    std::ifstream infile{argv[0]};
    if (!infile) {
        std::cerr << "Can't open design executable for frequency probe\n";
    } else {
        constexpr char tag[] = "Actual clock frequency:";
        const auto tag_len = std::strlen(tag);
        std::string line;
        while (std::getline(infile, line)) {
            auto pos = line.find(tag);
            if (pos == std::string::npos) continue;

            double f = 0;
            // Scan first floating point after tag; leading spaces OK
            if (std::sscanf(line.c_str() + pos + tag_len, " %lf", &f) == 1 && f > 0.0) {
                frequency = f * 1e6;
                std::printf("Design frequency: %.2f MHz\n", frequency/1e6);
                break;
            }
        }
    }

    printf("\n=== Estimated II per kernel\n");
    printf("Assuming Design frequency is %.2lf MHz\n", frequency / 1e6);

    printf("compute_kernel      ~ II = %lf\n", (res_kernel_compute.mean / 1e6 * frequency) / double(problem_size));
    printf("TOTAL FPGA compute  ~ II = %lf\n", (res_total_compute.mean / 1e6 * frequency) / double(problem_size));

    printf("\n");
    printf("Problem size : %.2e\n", double(problem_size));
    printf("FPGA RAM usage : %.2f GB (%.1f %%)\n", needed_mem_gb, double(needed_mem_gb / global_mem_gb * 100));

    const double th_kernel = (sizeof(DATATYPE) * double(problem_size) * 3) / (res_kernel_compute.mean / 1e6) / 1e9;
    printf("Estimated Throughput : %.3f GB/s (Kernel Load & Store streams - only fpga computation, so excluding copies)\n", th_kernel);
    const double th_memcpy_in = (sizeof(DATATYPE) * double(problem_size) * 2) / (res_cpu_to_fpga.mean / 1e6) / 1e9;
    const double th_memcpy_out = (sizeof(DATATYPE) * double(problem_size)) / (res_fpga_to_cpu.mean / 1e6) / 1e9;
    printf("Estimated Throughput : %.3f GB/s (only memcpy host_to_device, no fpga computation)\n", th_memcpy_in);
    printf("Estimated Throughput : %.3f GB/s (only memcpy device_to_host, no fpga computation)\n", th_memcpy_out);

    printf("Performance : %.3f Mega-items/s (only kernel, store stream)\n", (double(problem_size) / (res_kernel_compute.mean / 1e6)) / 1e6);

    sycl::free(d_A, queue);
    sycl::free(d_B, queue);
    sycl::free(d_res, queue);

    return 0;
}
