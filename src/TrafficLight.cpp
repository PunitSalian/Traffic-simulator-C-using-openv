#include "TrafficLight.h"
#include <iostream>
#include <random>

/* Implementation of class "MessageQueue" */

template <typename T> T MessageQueue<T>::receive() {
  // The method receive should use std::unique_lock<std::mutex> and
  // _condition.wait()
  // to wait for and receive new messages and pull them from the queue using
  // move semantics.
  // The received object should then be ret
  // std::this_thread::sleep_for(std::chrono::milliseconds(100));        //
  // perform vector modification under the lock
  std::unique_lock<std::mutex> uLock(_msgmutex);
  _cond.wait(uLock, [this] {
    return !_queue.empty();
  }); // pass unique lock to condition variable

  // remove last vector element from queue
  T currphase = std::move(_queue.back());
  _queue.pop_back();

  return currphase; // will not be copied due to return value optimization (RVO)
                    // in C++urned by the receive function.
}

template <typename T> void MessageQueue<T>::send(T &&msg) {
  // The method send should use the mechanisms std::lock_guard<std::mutex>
  // as well as _condition.notify_one() to add a new message to the queue and
  // afterwards send a notification.

  // perform queue modification under the lock
  std::lock_guard<std::mutex> uLock(_msgmutex);

  _queue.push_back(std::move(msg));

  _cond.notify_one(); // notify client after pushing new msg iin buffer
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight() { _currentPhase = TrafficLightPhase::RED; }

void TrafficLight::waitForGreen() {
  // add the implementation of the method waitForGreen, in which an
  // infinite while-loop
  // runs and repeatedly calls the receive function on the message queue.
  // Once it receives TrafficLightPhase::green, the method returns.
  while (true) {
    if (_messageq.receive() == GREEN) {
      return;
    }
  }
}

TrafficLightPhase TrafficLight::getCurrentPhase() { return _currentPhase; }

void TrafficLight::simulate() {
  // Finally, the private method „cycleThroughPhases“ should be
  // started in a thread when the public method „simulate“ is called. To do
  // this, use
  // the thread queue in the base class.
  threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}
double getRandomWaitTime() {
  /*
  Return a random number between and including 4000-6000 ms
  http://www.cplusplus.com/reference/random/uniform_int_distribution/
  */
  typedef std::chrono::high_resolution_clock myclock;
  myclock::time_point beginning = myclock::now();

  // obtain a seed from the timer
  myclock::duration d = myclock::now() - beginning;
  unsigned seed = d.count();

  std::mt19937 generator(seed);
  std::uniform_real_distribution<double> distribution(4, 8);
  return distribution(generator) * 1000;
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases() {

  double cyclewaitime = getRandomWaitTime();
  std::chrono::time_point<std::chrono::system_clock> lastupdatedtime =
      std::chrono::system_clock::now();
  // Implement the function with an infinite loop that measures the
  // time between two loop cycles
  // and toggles the current phase of the traffic light between red and green
  // and sends an update method
  // to the message queue using move semantics. The cycle duration should be a
  // random value between 4 and 6 seconds.
  while (true) {

    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    auto currtime = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::system_clock::now() - lastupdatedtime)
                        .count();

    if (currtime >= cyclewaitime) {

      cyclewaitime = getRandomWaitTime();

      if (_currentPhase == GREEN) {
        _currentPhase = RED;
      } else {
        _currentPhase = GREEN;
      }
      _messageq.send(std::move(_currentPhase));
      lastupdatedtime = std::chrono::system_clock::now();
    }
  }

  // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms
  // between two cycles.
}
