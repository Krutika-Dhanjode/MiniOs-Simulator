#ifndef MINIOS_DISK_SCHEDULER_H
#define MINIOS_DISK_SCHEDULER_H

#include <string>
#include <vector>

// Abstract disk scheduling strategy.
class DiskScheduler {
public:
    virtual ~DiskScheduler() = default;

    virtual std::string name() const = 0;

    // Returns the next cylinder to service given the current queue and head.
    virtual int pickNext(const std::vector<int>& queue, int head) = 0;
};

#endif // MINIOS_DISK_SCHEDULER_H
