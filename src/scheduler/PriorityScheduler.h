#ifndef MINIOS_PRIORITY_SCHEDULER_H
#define MINIOS_PRIORITY_SCHEDULER_H

#include "scheduler/Scheduler.h"

// Priority Scheduling (non-preemptive): the waiting process with the
// highest priority (lowest priority number) is dispatched first.
class PriorityScheduler : public Scheduler {
public:
    std::string name() const override { return "Priority"; }

    Process* pickNext(
        const std::vector<std::unique_ptr<Process>>& processes) override {
        Process* best = nullptr;
        for (const auto& p : processes) {
            if (p->getState() == ProcessState::TERMINATED) continue;
            if (!best || p->getPriority() < best->getPriority()) {
                best = p.get();
            }
        }
        return best;
    }
};

#endif // MINIOS_PRIORITY_SCHEDULER_H
