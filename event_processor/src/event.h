#pragma once

#include <iostream>
#include <thread>

#include "ievent.h"
#include "types.h"

class Event final: public IEvent
{
  public:
    Event(std::thread::id producer_thread_id, size_t count, int value)
      : producer_thread_id_(producer_thread_id)
      , count_(count)
      , value_(value) {}
    
    Event(const Event&) = delete;
    Event& operator=(const Event&) = delete;

    Event(Event&&) = delete;
    Event& operator=(Event&&) = delete;

    void Process() override
    {
      std::cout << "Event::Process: "
                << ",producer_thread_id=" << producer_thread_id_
                << ",count=" << count_
                << ",value=" << value_
                << '\n';
    }           

  private:
    std::thread::id producer_thread_id_;
    size_t count_;
    int value_;
};

class BigEvent final: public IEvent
{
  public:
    BigEvent(std::thread::id producer_thread_id, size_t count, int value)
      : producer_thread_id_(producer_thread_id)
      , count_(count)
    {
      values_[0] = value;
    }
    
    BigEvent(const BigEvent&) = delete;
    BigEvent& operator=(const BigEvent&) = delete;

    BigEvent(BigEvent&&) = delete;
    BigEvent& operator=(BigEvent&&) = delete;

    void Process() override
    {
      std::cout << "BigEvent::Process: "
                << ",producer_thread_id=" << producer_thread_id_
                << ",count=" << count_
                << ",value=" << values_[0]
                << '\n';
    }           

  private:
    std::thread::id producer_thread_id_;
    size_t count_;
    int values_[64] = {0};
};