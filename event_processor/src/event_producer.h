#pragma once

#include <atomic>
#include <memory>
#include <thread>

#include "event_processor.h"

/** Produces a number of events, then stops.
 */
class EventProducer final
{
 public:
  using type = EventProducer;
  using Ptr = std::shared_ptr<EventProducer>;
  using WeakPtr = std::weak_ptr<EventProducer>;

  template<typename... Args>
  static auto Create(Args &&... args)
  {
    auto obj = Ptr{new type(std::forward<Args>(args)...)};
    obj->Initialise();
    return obj;
  }

  ~EventProducer();

  EventProducer(const EventProducer &) = delete;
  EventProducer(EventProducer &&) noexcept = delete;
  EventProducer & operator=(const EventProducer &) = delete;
  EventProducer & operator=(EventProducer &&) noexcept = delete;

  void Start();
  void WaitToComplete();

 private:
  // Two stage construction.
  EventProducer(size_t num_of_events, EventProcessor::Ptr event_procesor);
  void Initialise();

  template <typename TEvent, typename... Args>
  void GenerateEvent(Args&&... args);

  template <typename TEvent, typename... Args>
  void GenerateEventRange(size_t size, Args&&... args);

  void EventPump();
  
  size_t num_of_events_;
  EventProcessor::Ptr event_processor_;
  std::atomic<bool> start_{false};
  std::atomic<bool> shutting_down_{false};
  std::thread event_pump_thread_;
  std::thread::id thread_id_;
};

