#ifndef MINIOS_ROUND_ROBIN_SCHEDULER_H
#define MINIOS_ROUND_ROBIN_SCHEDULER_H

#include <cstddef>
#include "scheduler/Scheduler.h"

// Round Robin: cyclic dispatch over all waiting processes.
class RoundRobinScheduler : public Scheduler {
public:
    std::string name() const override { return "Round Robin"; }

    Process* pickNext(
        const std::vector<std::unique_ptr<Process>>& processes) override {
        std::size_t n = processes.size();
        for (std::size_t i = 0; i < n; ++i) {
            std::size_t idx = (index_ + i) % n;
            if (processes[idx]->getState() != ProcessState::TERMINATED) {
                index_ = (idx + 1) % n;
                return processes[idx].get();
            }
        }
        return nullptr;
    }

private:
    std::size_t index_ = 0;
};

#endif // MINIOS_ROUND_ROBIN_SCHEDULER_H
