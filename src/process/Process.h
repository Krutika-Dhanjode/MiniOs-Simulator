#ifndef MINIOS_PROCESS_H
#define MINIOS_PROCESS_H

#include <string>
#include "process/PCB.h"

// Abstract base class for every execution context in the system.
// Deriving classes implement run(); the base class owns the lifecycle.
class Process {
public:
    Process(int pid, const std::string& name, int priority);
    virtual ~Process() = default;

    int getPid() const;
    const std::string& getName() const;
    ProcessState getState() const;
    int getPriority() const;
    const PCB& getPcb() const;

    // Total burst time of the process (used by SJF scheduling).
    // Captured at construction so SJF stays non-preemptive.
    unsigned long getTotalWork() const { return totalWork_; }

    // Template method: one scheduling slice. New -> Running -> Waiting,
    // or Running -> Terminated if the derived run() finished its work.
    void execute();
    void terminate();

protected:
    // Derived classes manipulate the PCB only through these accessors.
    PCB& pcb();
    void setTotalWork(unsigned long work) { totalWork_ = work; }
    void addCpuTime(unsigned long ticks);
    void advanceProgramCounter(unsigned long instructions);

    virtual void run() = 0; // pure virtual -> abstraction

private:
    Process(const Process&) = delete;
    Process& operator=(const Process&) = delete;

    PCB pcb_; // composition: every process has-a control block
    unsigned long totalWork_ = 0;
};

#endif // MINIOS_PROCESS_H
