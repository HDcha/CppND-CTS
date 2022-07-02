#include "TrafficLight.h"
#include <iostream>
#include <mutex>
#include <random>

/* Implementation of class "MessageQueue" */

template<typename T>
T &&MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait()
    // to wait for and receive new messages and pull them from the queue using move semantics.
    // The received object should then be returned by the receive function.
}

template<typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex>
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
    std::lock_guard<std::mutex> lock_guard(_mutex);
    _queue.template emplace_back(std::move(msg));
    _condition.notify_one();
}

/* Implementation of class "TrafficLight" */
TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

[[maybe_unused]] void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop
    // runs and repeatedly calls the receive function on the message queue.
    // Once it receives TrafficLightPhase::green, the method returns.
}

[[maybe_unused]] TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class.
    std::thread cycle_through_phases_thread(&TrafficLight::cycleThroughPhases, this);
    std::lock_guard<std::mutex> lock_guard(_mutex);
    this->threads.emplace_back(std::move(cycle_through_phases_thread));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles
    // and toggles the current phase of the traffic light between red and green and sends an update method
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds.
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles.

    //initialize clock and lock
    auto time_mark = std::chrono::system_clock::now();
    std::unique_lock<std::mutex> lock(_mutex, std::defer_lock);

    // initialize random number generator
    std::random_device dev;
    std::mt19937 random_number_generator(dev());
    std::uniform_int_distribution<> distribution_ms(4000, 6000);
    auto random_interval_ms{distribution_ms(random_number_generator)};

    // start cycling phase
    while (true) {
        if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - time_mark).count() > random_interval_ms) {
            random_interval_ms = distribution_ms(random_number_generator);
            lock.lock();
            _currentPhase = _currentPhase == TrafficLightPhase::green ? TrafficLightPhase::red : TrafficLightPhase::green;
            _traffic_light_queue.send(TrafficLightPhase(_currentPhase)); // no std::move necessary, since we use an r-value
            lock.unlock();
            time_mark = std::chrono::system_clock::now();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
