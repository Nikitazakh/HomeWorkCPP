//
// Created by zahar on 23.03.2018.
//

#include <iostream>
#include <list>
#include <utility>
#include <vector>
#include <bits/unique_ptr.h>
#include <future>
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <climits>

template <typename T>
std::list <T> parallel_quick_sort(std::list <T> input)
{
    if (input.empty())
    {
        return input;
    }
    std::list <T> result;
    result.splice(result.begin(), input, input.begin());
    const T & ref_point = *result.begin();
    auto bound = std::partition(input.begin(), input.end(), [&](const T & t){return t<ref_point;});

    std::list <T> any_list;
    any_list.splice(any_list.begin(),input, input.begin(), bound);

    std::future<std::list<T>> any_future = std::async(&parallel_quick_sort<T>, std::move(any_list));

    std::list<T> new_list_high = parallel_quick_sort(std::move(input));

    result.splice(result.end(), new_list_high);
    result.splice(result.begin(), any_future.get());
    return result;
}

template <typename T>
std::list <T> quick_sort(std::list <T> input)
{
    if (input.empty())
    {
        return input;
    }
    std::list <T> result;
    result.splice(result.begin(), input, input.begin());
    const T & ref_point = *result.begin();
    auto bound = std::partition(input.begin(), input.end(), [&](const T & t){return t<ref_point;});

    std::list <T> any_list;
    any_list.splice(any_list.begin(),input, input.begin(), bound);

    std::list<T> any_future = quick_sort(any_list);
    std::list<T> new_list_high = quick_sort(input);

    result.splice(result.end(), new_list_high);
    result.splice(result.begin(), any_future);
    return result;
}


int main(int argc, char ** argv)
{
    std::list<int> need_to_sort;
    need_to_sort.push_back(10);
    need_to_sort.push_back(4);
    need_to_sort.push_back(7);
    need_to_sort.push_back(1);
    need_to_sort.push_back(15);
    need_to_sort.push_back(6);
    need_to_sort.push_back(7);
    need_to_sort.push_back(13);
    need_to_sort.push_back(2);
    need_to_sort.push_back(8);

    std::chrono::time_point<std::chrono::system_clock> start_par, end_par;
    start_par = std::chrono::system_clock::now();
    std::list<int> res_par = parallel_quick_sort(need_to_sort);
    end_par = std::chrono::system_clock::now();

    std::chrono::time_point<std::chrono::system_clock> start_q, end_q;
    start_q = std::chrono::system_clock::now();
    std::list<int> res_q = quick_sort(need_to_sort);
    end_q = std::chrono::system_clock::now();

    int elapsed_seconds_par = std::chrono::duration_cast<std::chrono::microseconds>(end_par-start_par).count();
    int elapsed_seconds_q = std::chrono::duration_cast<std::chrono::microseconds>(end_q-start_q).count();

    std::cout << "Time for parallel: " << elapsed_seconds_par << " mks\n";
    std::cout << "Time for quick: " << elapsed_seconds_q << " mks\n";

}
