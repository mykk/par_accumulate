#include <algorithm>
#include <vector>
#include <thread>
#include <execution>

namespace parallel {
    template <class AccumulateExecutionPolicy, class ReduceExecutionPolicy, class InputIt, class InitF, class AccumulateF, class ReduceF>
    [[nodiscard]] auto accumulate(AccumulateExecutionPolicy&& policyAccumulate, ReduceExecutionPolicy&& policyReduce, InputIt first, InputIt last, InitF initF, AccumulateF accumulateF, ReduceF reduceF) -> decltype(initF()) {
        auto const size = last - first;
        auto const thread_count = std::thread::hardware_concurrency();

        auto const result_count = thread_count < size ? thread_count : size;
        auto const chunk_size = size / result_count;

        auto results = std::vector<decltype(initF())>(result_count);
        std::generate(results.begin(), results.end(), initF);

        std::for_each(std::forward<AccumulateExecutionPolicy>(policyAccumulate), results.begin(), results.end(), [chunk_size, result_count, first, last, results_start = &results[0], &accumulateF](auto& temp_result) {
            auto const thread_index = &temp_result - results_start;
            auto const start = first + thread_index * chunk_size;
            auto const end = (thread_index == result_count - 1) ? last : start + chunk_size;

            for (auto it = start; it != end; ++it) {
                accumulateF(temp_result, *it);
            }
        });

        return std::reduce(std::forward<ReduceExecutionPolicy>(policyReduce), results.begin(), results.end(), initF(), reduceF);
    }
}