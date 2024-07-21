#pragma once

#include <atomic>
#include <vector>

#include "atomic_utils.h"
#include "reserved_event.h"

/** Singleton class.
 ** Based on a circular buffer.
 ** CAPACITY has to be a binary number.
 */
template<size_t CAPACITY, size_t EVENT_SIZE>
class EventAllocator final
{
 public:
  static constexpr size_t Capacity = CAPACITY;
  static constexpr size_t EventSize = EVENT_SIZE;

  static EventAllocator & Instance()
  {
    static EventAllocator the_instance{};
    return the_instance;
  }

  ReservedEvent Alloc()
  {
    size_t retry = Capacity;
    while(retry--)
    {
      auto next_free_id_hint = AtomicIncrementAndWrap(next_free_id_hint_, Capacity);
      auto & element = pool_[next_free_id_hint];
      bool expected = true;
      if(element.is_free_.compare_exchange_strong(expected, false))
      {
        size_t id = next_free_id_hint;
        return ReservedEvent{id, element.buff_};
      }
    }
    return ReservedEvent{0, nullptr};
  }

  void Free(size_t id)
  {
    pool_[id].is_free_.store(true);
  }

  void * GetEvent(size_t id)
  {
    return pool_[id].buff_;
  }

 private:
  EventAllocator() = default;

  struct Element
  {
    std::atomic<bool> is_free_ = true;
    std::byte buff_[EventSize];
  };

  Element pool_[Capacity];
  std::atomic<size_t> next_free_id_hint_ = 0;  
};

/** Singleton class.
 ** Based on a circular buffer.
 ** CAPACITY has to be a binary number.
 */
template<size_t CAPACITY, size_t EVENT_SIZE, size_t COUNT>
class EventRangeAllocator final
{
 public:
  static constexpr size_t Capacity = CAPACITY;
  static constexpr size_t EventSize = EVENT_SIZE;
  static constexpr size_t Count = COUNT;

  static EventRangeAllocator & Instance()
  {
    static EventRangeAllocator the_instance{};
    return the_instance;
  }

  std::vector<ReservedEvents> Alloc(size_t num_of_events)
  {
    std::vector<ReservedEvents> reserved_events;
    reserved_events.reserve(64);

    size_t retry = Capacity;
    while(retry-- && num_of_events)
    {
      auto next_free_id_hint = AtomicIncrementAndWrap(next_free_id_hint_, Capacity);
      auto & element = pool_[next_free_id_hint];
      bool expected = true;
      if(element.is_free_.compare_exchange_strong(expected, false))
      {
        size_t id = next_free_id_hint;
        if(num_of_events >= Count)
        {
          num_of_events -= Count;
          reserved_events.emplace_back(id, element.buff_, Count, EventSize);
        }
        else
        {
          reserved_events.emplace_back(id, element.buff_, num_of_events, EventSize);
          return reserved_events;
        }
      }
    }
    return reserved_events;
  }

  void Free(size_t id)
  {
    pool_[id].is_free_.store(true);
  }

  void * GetEvent(size_t id)
  {
    return pool_[id].buff_;
  }

 private:
  EventRangeAllocator() = default;

  struct Element
  {
    std::atomic<bool> is_free_ = true;
    std::byte buff_[EventSize * Count];
  };

  Element pool_[Capacity];
  std::atomic<size_t> next_free_id_hint_ = 0;  
};