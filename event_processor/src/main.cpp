#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <vector>

#include "event_processor.h"
#include "event_producer.h"

int main(int argc, char *argv[])
{
  std::copy(argv, argv + argc, std::ostream_iterator<char *>(std::cout, "\n"));
  if(3 != argc)
  {
    std::cout << "Usage "
              << argv[0] << " "
              << "<num of producers> <number of events>" << '\n';
    return 0;
  }

  const size_t numof_producers = atoi(argv[1]);
  const size_t numof_events = atoi(argv[2]);
  if(!numof_producers || !numof_events)
  {
    std::cout << "Invalid command line arguments" << '\n';
    return 0;
  }

  auto event_processor = EventProcessor::Create();

  std::vector<EventProducer::Ptr> producers;
  producers.reserve(numof_producers);
  for(size_t i = 0; i < numof_producers; ++i)
  {
    producers.emplace_back(EventProducer::Create(numof_events, event_processor));
  }

  for(const auto & p: producers)
  {
    p->Start();
  }

  for(const auto & p: producers)
  {
    p->WaitToComplete();
  }
  producers.clear();

  event_processor.reset();

  return 0;
}