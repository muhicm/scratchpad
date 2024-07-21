#include <iostream>

#include "event_producer.h"
#include "event.h"

EventProducer::EventProducer(size_t num_of_events, EventProcessor::Ptr event_procesor)
  : num_of_events_(num_of_events)
  , event_processor_(std::move(event_procesor))
{}

void EventProducer::Initialise()
{
  event_pump_thread_ = std::thread(&EventProducer::EventPump, this);
  thread_id_ = event_pump_thread_.get_id();
}

EventProducer::~EventProducer()
{
  shutting_down_.store(true);
  if(event_pump_thread_.joinable())
  {
    event_pump_thread_.join();
  }
}

void EventProducer::Start()
{
  start_.store(true);
}

void EventProducer::WaitToComplete()
{
  if(event_pump_thread_.joinable())
  {
    event_pump_thread_.join();
  }
}

template <typename TEvent, typename... Args>
void EventProducer::GenerateEvent(Args&&... args)
{
  auto reserved_event = event_processor_->Reserve<TEvent>(thread_id_, std::forward<Args>(args)...);
  if(reserved_event.IsValid())
  {
    event_processor_->Commit(reserved_event.GetSequenceNumber());
  }
  else
  {
    std::cout << "ERROR: GenerateEvent: Reserve failed" << '\n';
  }
}

template <typename TEvent, typename... Args>
void EventProducer::GenerateEventRange(size_t size, Args&&... args)
{
  auto reserved_events_collection = event_processor_->ReserveRange(size);
  if(!reserved_events_collection.empty())
  {
    for(auto& reserved_events: reserved_events_collection)
    {
      if(reserved_events.IsValid())
      {
        for(size_t i = 0; i < reserved_events.Count(); ++i)
        {
          reserved_events.Emplace<TEvent>(i, thread_id_, std::forward<Args>(args)...);
        }
        event_processor_->Commit(reserved_events.GetSequenceNumber(), reserved_events.Count());
      }
      else
      {
        std::cout << "ERROR: GenerateEventRange: Reserve failed" << '\n';
      }
    }
  }
  else
  {
    std::cout << "ERROR: GenerateEventRange: ReserveRange failed" << '\n';
  }
}

void EventProducer::EventPump()
{
  while(!start_.load());

  //std::cout << "EventProducer: " << thread_id_ << " started" << '\n';

  for(size_t i = 0; (i < num_of_events_) && !shutting_down_.load(); ++i)
  {
    switch(i&3)
    {
      case 0:
        GenerateEvent<Event>(i, 1010);
      break;
      case 1:
        GenerateEventRange<Event>(10UL, i, 1020);
      break;
      case 2:
        GenerateEvent<BigEvent>(i, 5010);
      break;
      case 3:
        GenerateEventRange<BigEvent>(23UL, i, 5020);
      break;
    }
  }

  //std::cout << "EventProducer: " << thread_id_ << " finished" << '\n';
}