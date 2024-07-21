#include <iostream>

#include "event_processor.h"
#include "event_allocator.h"
#include "mpsc_ring_buffer.h"
#include "event.h"

namespace
{

template <typename ...TEvents>
static constexpr size_t MAX_EVENT_SIZE = std::max(sizeof(TEvents)...);

static constexpr size_t CAPACITY = 65536;
static constexpr size_t EVENT_SIZE = MAX_EVENT_SIZE<Event, BigEvent>;
static constexpr size_t COUNT = 16;
using MyEventAllocator = EventAllocator<CAPACITY, EVENT_SIZE>;
using MyEventRangeAllocator = EventRangeAllocator<CAPACITY, EVENT_SIZE, COUNT>;

/** Element of the producers/consumer queue.
 ** Sequence number is considered to be event id, used to identify event in the allocator, rather than message
 ** sequence number that increments sequentially.
 ** Count can be 0 which means that event memory came from EventAllocator rather than EventRangeAllocator.
 ** 
 */
struct QElement
{
  Integer sequence_number_;
  size_t count_;
};

using MyQueue = MPSCRingBuffer<QElement, (CAPACITY * 4)>;

} // namespace

void EventProcessor::Initialise()
{
  event_pump_thread_ = std::thread(&EventProcessor::EventPump, this);
}

EventProcessor::~EventProcessor()
{
  shutting_down_.store(true);
  if(event_pump_thread_.joinable())
  {
    event_pump_thread_.join();
  }
}

bool EventProcessor::ProcessEvent()
{
  auto qevent = typename MyQueue::Element_t{};
  bool ok = MyQueue::Instance().Read(qevent);
  if(ok)
  {
    std::cout << "EventProcessor:ProcessEvent: event_id="
                << qevent.sequence_number_ << '\n';
    if(!qevent.count_)
    { // single event
      auto event = MyEventAllocator::Instance().GetEvent(qevent.sequence_number_);
      static_cast<IEvent *>(event)->Process();
      MyEventAllocator::Instance().Free(qevent.sequence_number_);
    }
    else
    { // event range
      auto events = MyEventRangeAllocator::Instance().GetEvent(qevent.sequence_number_);
      for(size_t i = 0; i < qevent.count_; ++i)
      {
        void* event = &static_cast<std::byte *>(events)[i * MyEventRangeAllocator::EventSize];
        static_cast<IEvent *>(event)->Process();
      }
      MyEventRangeAllocator::Instance().Free(qevent.sequence_number_);
    }
  }
  return ok;
}

void EventProcessor::EventPump()
{
  std::cout << "EventProcessor: started" << '\n';

  while(!shutting_down_.load())
  {
    ProcessEvent();
  }

  // Drain the event queue.
  while(ProcessEvent());
  
  std::cout << "EventProcessor: finished" << '\n';
}

ReservedEvent EventProcessor::ReserveEvent()
{
  if(!shutting_down_.load())
  {
    return MyEventAllocator::Instance().Alloc();
  }
  return ReservedEvent{};
}

std::vector<ReservedEvents> EventProcessor::ReserveRange(const size_t size)
{
  if(!shutting_down_.load())
  {
    return MyEventRangeAllocator::Instance().Alloc(size);
  }
  return std::vector<ReservedEvents>{};
}

void EventProcessor::Commit(const Integer sequence_number)
{
  auto e = QElement{sequence_number, 0UL};
  while(!MyQueue::Instance().Write(e));
}

void EventProcessor::Commit(const Integer sequence_number, const size_t count)
{
  auto e = QElement{sequence_number, count};
  while(!MyQueue::Instance().Write(e));
}
