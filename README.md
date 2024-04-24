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
Possible output:

```
g++ -O3 -std=c++17 -Wall -Wextra -pedantic par_accumulate.cpp -ltbb
./a.out

seq duration:328.791
par duration:38.5968

w: 9997787
j: 10001929
m: 10005369
r: 9994678
k: 9995113
s: 10002794
p: 10001025
c: 10000225
y: 10001047
l: 10000179
i: 10003384
v: 10000093
d: 10005290
q: 9997664
b: 10000615
o: 9998284
n: 10002500
a: 9991518
u: 10005686
h: 10000507
g: 9998428
t: 10002651
x: 9997021
f: 9995883
e: 10000330

d: 10005290
o: 9998284
a: 9991518
q: 9997664
u: 10005686
g: 9998428
t: 10002651
x: 9997021
f: 9995883
h: 10000507
e: 10000330
w: 9997787
i: 10003384
v: 10000093
l: 10000179
y: 10001047
c: 10000225
p: 10001025
s: 10002794
k: 9995113
r: 9994678
m: 10005369
j: 10001929
n: 10002500
b: 10000615
```
