#pragma once

#include <atomic>
#include <memory>
#include <thread>

#include "ievent_processor.h"

class EventProcessor final: public IEventProcessor<EventProcessor>
{
 public:
  using type = EventProcessor;
  using Ptr = std::shared_ptr<EventProcessor>;
  using WeakPtr = std::weak_ptr<EventProcessor>;
  
 public:
  template<typename... Args>
  static auto Create(Args &&... args)
  {
    auto obj = Ptr{new type(std::forward<Args>(args)...)};
    obj->Initialise();
    return obj;
  }

  ~EventProcessor();

  EventProcessor(const EventProcessor &) = delete;
  EventProcessor(EventProcessor &&) noexcept = delete;
  EventProcessor & operator=(const EventProcessor &) = delete;
  EventProcessor & operator=(EventProcessor &&) noexcept = delete;

  ReservedEvent ReserveEvent();
  std::vector<ReservedEvents> ReserveRange(const size_t size);
  void Commit(const Integer sequence_number);
  void Commit(const Integer sequence_number, const size_t count);

 private:
  // Two stage construction.
  EventProcessor() = default;
  void Initialise();

  [[maybe_unused]] bool ProcessEvent();
  void EventPump();

  std::atomic<bool> shutting_down_{false};
  std::thread event_pump_thread_;
};