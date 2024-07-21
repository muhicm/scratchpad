#pragma once

template <template <typename> class CreationPolicy, typename T>
class Constructor
{
 public:
  template <typename ...Args>
  static void Construct(void * ptr, Args&&... args)
  {
    CreationPolicy<T>::Construct(ptr, std::forward<Args>(args)...);
  }
};

template <typename T>
class NewPlacementConstructor
{
 public:
  template <typename ...Args>
  static void Construct(void * ptr, Args&&... args)
  {
    new (ptr) T(std::forward<Args>(args)...);
  }
};