#pragma once

#include "constructor.h"
#include "types.h"

struct ReservedEvent
  {
    ReservedEvent() : ReservedEvent(0UL, nullptr) {}
    ReservedEvent(const Integer sequence_number, void *const event)
      : sequence_number_{sequence_number}
      , event_{event}
    {}

    // @todo - Compiler uses return value copy elision, however it also complains
    // if copy/move special members are declared deleted.

    //ReservedEvent(const ReservedEvent&) = delete;
    //ReservedEvent& operator=(const ReservedEvent&) = delete;

    //ReservedEvent(ReservedEvent&&) = delete;
    //ReservedEvent& operator=(ReservedEvent&&) = delete;

    Integer GetSequenceNumber()  const {return sequence_number_;}
    void* GetEvent() const {return event_;}

    bool IsValid() const {return event_;}

    private:
    const Integer sequence_number_;
    void *const event_;
  };

  struct ReservedEvents
  {
    ReservedEvents(const Integer sequence_number, void *const event, const size_t count, const size_t event_size)
      : sequence_number_{sequence_number}
      , events_{event}
      , count_{count}
      , event_size_{event_size}
    {}

    // @todo - Compiler uses return value copy elision, however it also complains
    // if copy/move special members are declared deleted.
    
    //ReservedEvents(const ReservedEvents&) = delete;
    //ReservedEvents& operator=(const ReservedEvents&) = delete;

    //ReservedEvents(ReservedEvents&&) = delete;
    //ReservedEvents& operator=(ReservedEvents&&) = delete;

    template <class TEvent, template <class> class Constructor, class ...Args>
    void Emplace(const size_t index, Args&&... args)
    {
      if(void *const event = GetEvent(index); event)
      {
        Constructor<TEvent>::Construct(event, std::forward<Args>(args)...);
      }
    }

    template <class TEvent, class ...Args>
    void Emplace(const size_t index, Args&&... args)
    {
      if(void *const event = GetEvent(index); event)
      {
        NewPlacementConstructor<TEvent>::Construct(event, std::forward<Args>(args)...);
      }
    }

    Integer GetSequenceNumber() const {return sequence_number_;}

    void *const GetEvent(const size_t index) const
    {
      if(index >= count_) return nullptr;
      return &static_cast<std::byte *>(events_)[index * event_size_];
    };

    size_t Count() const {return count_;}

    bool IsValid() const {return events_ && count_;}

   private:
    const Integer sequence_number_;
    void *const events_;
    const size_t count_;
    const size_t event_size_;
  };