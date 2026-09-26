#ifndef MINIOS_SCHEDULER_H
#define MINIOS_SCHEDULER_H

#include <memory>
#include <string>
#include <vector>
#include "process/Process.h"

// Abstract strategy for CPU scheduling algorithms.
class Scheduler {
public:
    virtual ~Scheduler() = default;

    virtual std::string name() const = 0;

    // Returns the next process to receive a time slice, or nullptr if none.
    virtual Process* pickNext(
        const std::vector<std::unique_ptr<Process>>& processes) = 0;
};

#endif // MINIOS_SCHEDULER_H
