#pragma once

#include <atomic>

#include "atomic_utils.h"

/** Singleton class.
 ** Multiple Producers Single Consumer circular buffer.
 ** CAPACITY has to be a binary number.
 */
template<typename Element, size_t CAPACITY>
class MPSCRingBuffer final
{
 public:
  using Element_t = Element;
  static constexpr size_t Capacity = CAPACITY;
  static constexpr size_t ElementSize = sizeof(Element);

  static MPSCRingBuffer & Instance()
  {
    static MPSCRingBuffer the_instance{};
    return the_instance;
  }

  bool Write(const Element & e)
  {
    if(count_.load() < Capacity)
    {
      lk_.Lock();
      if(count_.load() < Capacity)
      {
        auto w = AtomicIncrementAndWrap(write_, Capacity);
        pool_[w] = e;
        count_.fetch_add(1);
      }
      lk_.UnLock();
      return true;
    }
    return false;
  }
  
  bool Read(Element & e)
  {
    if(count_.load())
    {
      auto r = read_;
      read_ = (r + 1) & (Capacity - 1);
      e = pool_[r];
      count_.fetch_sub(1);
      return true;
    }
    return false;
  }

 private:
  MPSCRingBuffer() = default;

  Element pool_[Capacity];
  size_t read_ = 0;
  std::atomic<size_t> write_ = 0;
  std::atomic<size_t> count_ = 0;
  AtomicSpinLock lk_;
};