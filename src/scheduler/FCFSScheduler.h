#ifndef MINIOS_FCFS_SCHEDULER_H
#define MINIOS_FCFS_SCHEDULER_H

#include "scheduler/Scheduler.h"

// First-Come, First-Served (non-preemptive): the earliest-created
// process is dispatched first and runs to completion.
class FCFSScheduler : public Scheduler {
public:
    std::string name() const override { return "FCFS"; }

    Process* pickNext(
        const std::vector<std::unique_ptr<Process>>& processes) override {
        for (const auto& p : processes) {
            if (p->getState() != ProcessState::TERMINATED) return p.get();
        }
        return nullptr;
    }
};

#endif // MINIOS_FCFS_SCHEDULER_H
