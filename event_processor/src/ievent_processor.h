#pragma once

#include <vector>

#include "constructor.h"
#include "reserved_event.h"
#include "types.h"

template <typename TEventProcessor>
class IEventProcessor
{
 public:
  ReservedEvent ReserveEvent()
  {
    return static_cast<TEventProcessor &>(*this).ReserveEvent();
  }

  template <class T, template <class> class Constructor, class ...Args>
  ReservedEvent Reserve(Args&&... args)
  {
    auto reserved_event = ReserveEvent();
    if(reserved_event.IsValid())
    {
      Constructor<T>::Construct(reserved_event.GetEvent(), std::forward<Args>(args)...);
    }
    return reserved_event;
  }

  template <class T, class ...Args>
  ReservedEvent Reserve(Args&&... args)
  {
    auto reserved_event = ReserveEvent();
    if(reserved_event.IsValid())
    {
      NewPlacementConstructor<T>::Construct(reserved_event.GetEvent(), std::forward<Args>(args)...);
    }
    return reserved_event;
  }

  std::vector<ReservedEvents> ReserveRange(const size_t size)
  {
    return static_cast<TEventProcessor &>(*this).ReserveRange(size);
  }

  void Commit(const Integer sequence_number)
  {
    return static_cast<TEventProcessor &>(*this).Commit(sequence_number);
  }
  
  void Commit(const Integer sequence_number, const size_t count)
  {
    return static_cast<TEventProcessor &>(*this).Commit(sequence_number, count);
  }

 private:
  IEventProcessor() = default;
  friend TEventProcessor;
};