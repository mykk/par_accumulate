# par_accumulate
Simple templated parallel accumulate function

Example usage
```
#include "par_accumulate/par_accumulate.h"

#include <unordered_map>
#include <iostream>
#include <chrono>
#include <random>
#include <numeric>
#include <algorithm>

std::unordered_map<char, size_t> count_chars(std::vector<char> const& chars) {
    std::unordered_map<char, size_t> char_counts{};

    for (auto c : chars) {
        ++char_counts[c];
    }
    return char_counts;
}

int main()
{
    static constexpr size_t total_count = 10000000 * ('z' - 'a');
    
    std::vector<char> chars{};
    chars.reserve(total_count);

    for (size_t i = 0; i < total_count; ++i) {
        chars.emplace_back('a' + (rand() % ('z' - 'a')));
    }

    auto start_par = std::chrono::steady_clock::now();
    auto char_count_par = parallel::accumulate(std::execution::par_unseq, std::execution::par_unseq, chars.begin(), chars.end(), 
                                     [](){ return std::unordered_map<char, size_t>{}; },
                                     [](auto& char_count, auto c) { ++char_count[c]; },
                                     [](auto const& lhs, auto const& rhs) {
                                        std::unordered_map<char, size_t> char_count{};
                                        for (auto const& val: lhs)
                                            char_count[val.first] = val.second;
                                        for (auto const& val: rhs)
                                            char_count[val.first] += val.second;

                                        return char_count;
                                     });
    auto end_par = std::chrono::steady_clock::now();

    auto start_seq = std::chrono::steady_clock::now();
    auto char_count_seq = count_chars(chars);
    auto end_seq = std::chrono::steady_clock::now();

    std::cout << "seq duration:" << std::chrono::duration<double, std::milli>(end_seq - start_seq).count() << std::endl;
    std::cout << "par duration:" << std::chrono::duration<double, std::milli>(end_par - start_par).count() << std::endl;

    for (auto const& counter : char_count_par) {
        std::cout << counter.first << ": " << counter.second << std::endl;
    }

    std::cout << std::endl;

    for (auto const& counter : char_count_seq) {
        std::cout << counter.first << ": " << counter.second << std::endl;
    }
}
```
