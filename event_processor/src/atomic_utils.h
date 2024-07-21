#pragma once

/** Atomically increment and limit the value.
 ** Note that wrap_limit must be on the binary boundary.
 */
template<typename T>
T AtomicIncrementAndWrap(std::atomic<T>& shared, T wrap_limit)
{
  static_assert(std::is_integral_v<T>, "Integral required.");
  
  --wrap_limit;
  T old_value = shared.load();
  T new_value;
  do
  {
    new_value = (old_value + 1) & wrap_limit;
  }
  while (!shared.compare_exchange_weak(old_value, new_value));
  return old_value;
}

class AtomicSpinLock
{
 public:
  AtomicSpinLock(): lk_(1) {}

  void Lock()
  {
    auto old_value = lk_.load();
    while(old_value == 0 || !lk_.compare_exchange_strong(old_value, old_value - 1))
    {
      old_value = lk_.load();
    }
  }

  void UnLock()
  {
      lk_.fetch_add(1);
  }

 private :
  std::atomic<int> lk_ ;
};