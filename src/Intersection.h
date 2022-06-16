#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "TrafficObject.h"
#include <future>
#include <memory>
#include <mutex>
#include <vector>

// forward declarations to avoid include cycle
class Street;
class Vehicle;

// auxiliary class to queue and dequeue waiting vehicles in a thread-safe manner
class WaitingVehicles
{
public:
    // getters / setters
    int getSize();

    // typical behaviour methods
    void pushBack(const std::shared_ptr<Vehicle> &vehicle, std::promise<void> &&promise);
    void permitEntryToFirstInQueue();

private:
    std::vector<std::shared_ptr<Vehicle>> _vehicles; // list of all vehicles waiting to enter this intersection
    std::vector<std::promise<void>> _promises;       // list of associated promises
    std::mutex _mutex;
};

class Intersection : public TrafficObject
{
public:
    // constructor / desctructor
    Intersection();

    // getters / setters
    void setIsBlocked(bool isBlocked);

    // typical behaviour methods
    void addVehicleToQueue(const std::shared_ptr<Vehicle> &vehicle);
    void addStreet(const std::shared_ptr<Street> &street);
    std::vector<std::shared_ptr<Street>> queryStreets(const std::shared_ptr<Street> &incoming); // return pointer to current list of all outgoing streets
    void simulate() override;
    void vehicleHasLeft([[maybe_unused]] const std::shared_ptr<Vehicle> &vehicle);
    static bool trafficLightIsGreen();

private:
    // typical behaviour methods
    void processVehicleQueue();

    // private members
    std::vector<std::shared_ptr<Street>> _streets; // list of all streets connected to this intersection
    WaitingVehicles _waitingVehicles;              // list of all vehicles and their associated promises waiting to enter the intersection
    bool _isBlocked;                               // flag indicating wether the intersection is blocked by a vehicle
};

#endif
