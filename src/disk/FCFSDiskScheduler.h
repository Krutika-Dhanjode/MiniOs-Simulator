#ifndef MINIOS_FCFS_DISK_SCHEDULER_H
#define MINIOS_FCFS_DISK_SCHEDULER_H

#include "disk/DiskScheduler.h"

// First-Come-First-Served: service requests in arrival order.
class FCFSDiskScheduler : public DiskScheduler {
public:
    std::string name() const override { return "FCFS"; }

    int pickNext(const std::vector<int>& queue, int) override {
        return queue.front();
    }
};

#endif // MINIOS_FCFS_DISK_SCHEDULER_H
