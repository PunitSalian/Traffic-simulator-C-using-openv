#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include "TrafficObject.h"
#include <condition_variable>
#include <deque>
#include <mutex>

// forward declarations to avoid include cycle
class Vehicle;

// FP.3 Define a class „MessageQueue“ which has the public methods send and
// receive.
// Send should take an rvalue reference of type TrafficLightPhase whereas
// receive should return this type.
// Also, the class should define an std::dequeue called _queue, which stores
// objects of type TrafficLightPhase.
// Also, there should be an std::condition_variable as well as an std::mutex as
// private members.

template <class T> class MessageQueue {
public:
  T receive();
  void send(T &&currphase);

private:
  std::mutex _msgmutex;
  std::condition_variable _cond;
  std::deque<T>
      _queue; // list of all vehicles waiting to enter this intersection
};

enum TrafficLightPhase { GREEN, RED };

class TrafficLight : public TrafficObject {
public:
  // constructor / desctructor
  TrafficLight() {}
  // getters / setters

  // typical behaviour methods
  void waitForGreen();
  void simulate();
  TrafficLightPhase getCurrentPhase();

private:
  // typical behaviour methods
  // FP.4b : create a private member of type MessageQueue for messages of type
  // TrafficLightPhase
  // and use it within the infinite loop to push each new TrafficLightPhase into
  // it by calling
  // send in conjunction with move semantics.
  void cycleThroughPhases();
  TrafficLightPhase _currentPhase;
  std::condition_variable _condition;
  std::mutex _mutex;
};

#endif