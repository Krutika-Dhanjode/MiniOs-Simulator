#ifndef MINIOS_SJF_SCHEDULER_H
#define MINIOS_SJF_SCHEDULER_H

#include "scheduler/Scheduler.h"

// Shortest Job First (non-preemptive): the waiting process with the
// smallest total burst time is dispatched first and runs to completion.
class SJFScheduler : public Scheduler {
public:
    std::string name() const override { return "SJF"; }

    Process* pickNext(
        const std::vector<std::unique_ptr<Process>>& processes) override {
        Process* best = nullptr;
        for (const auto& p : processes) {
            if (p->getState() == ProcessState::TERMINATED) continue;
            if (!best || p->getTotalWork() < best->getTotalWork()) {
                best = p.get();
            }
        }
        return best;
    }
};

#endif // MINIOS_SJF_SCHEDULER_H
