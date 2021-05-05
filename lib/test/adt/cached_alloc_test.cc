/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/adt/pool/cached_alloc.h"
#include "srsran/common/test_common.h"
#include <chrono>

void test_cached_deque_basic_operations()
{
  srsran::deque<int> my_deque;
  TESTASSERT(my_deque.empty() and my_deque.size() == 0);
  my_deque.push_front(0);
  my_deque.push_back(1);
  TESTASSERT(my_deque.front() == 0 and my_deque.back() == 1);
  TESTASSERT(my_deque.size() == 2);

  srsran::deque<int> my_deque2(my_deque);
  TESTASSERT(my_deque == my_deque2);
  my_deque.clear();
  TESTASSERT(my_deque != my_deque2);
  TESTASSERT(my_deque2.size() == 2 and my_deque2.back() == 1);
  TESTASSERT(my_deque.empty());

  my_deque = my_deque2;
  TESTASSERT(my_deque == my_deque2);
  my_deque2.clear();
  TESTASSERT(my_deque2.empty());

  my_deque2 = std::move(my_deque);
  TESTASSERT(my_deque.empty() and my_deque2.size() == 2);
}

struct C {
  C()             = default;
  C(C&&) noexcept = default;
  C(const C&)     = delete;
  C& operator=(C&&) noexcept = default;
  C& operator=(const C&) = delete;

  bool operator==(const C& other) { return true; }
};

void test_cached_queue_basic_operations()
{
  srsran::queue<C> my_queue;
  TESTASSERT(my_queue.empty());
  my_queue.push(C{});
  TESTASSERT(my_queue.size() == 1);

  srsran::queue<C> my_queue2(std::move(my_queue));
  TESTASSERT(my_queue2.size() == 1);
}

void cached_deque_benchmark()
{
  using std::chrono::high_resolution_clock;
  using std::chrono::microseconds;

  srsran::deque<int>                my_deque;
  std::deque<int>                   std_deque;
  high_resolution_clock::time_point tp;

  size_t N = 10000000, n_elems = 10;

  for (size_t i = 0; i < n_elems; ++i) {
    my_deque.push_back(i);
    std_deque.push_back(i);
  }

  // NOTE: this benchmark doesnt account for when memory is fragmented
  tp = high_resolution_clock::now();
  for (size_t i = n_elems; i < N; ++i) {
    std_deque.push_back(i);
    std_deque.pop_front();
  }
  microseconds t_std = std::chrono::duration_cast<microseconds>(high_resolution_clock::now() - tp);

  tp = high_resolution_clock::now();
  for (size_t i = n_elems; i < N; ++i) {
    my_deque.push_back(i);
    my_deque.pop_front();
  }
  microseconds t_cached = std::chrono::duration_cast<microseconds>(high_resolution_clock::now() - tp);

  fmt::print("Time elapsed: cached alloc={} usec, std alloc={} usec", t_cached.count(), t_std.count());
}

int main()
{
  test_cached_deque_basic_operations();
  test_cached_queue_basic_operations();
  cached_deque_benchmark();
  return 0;
}