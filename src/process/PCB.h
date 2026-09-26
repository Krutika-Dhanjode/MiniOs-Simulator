#ifndef MINIOS_PCB_H
#define MINIOS_PCB_H

#include <string>
#include "process/ProcessState.h"

// Process Control Block: encapsulates all kernel bookkeeping for one process.
class PCB {
public:
    PCB(int pid, const std::string& name, int priority);

    int getPid() const;
    const std::string& getName() const;
    int getPriority() const;
    ProcessState getState() const;
    unsigned long getCpuTimeUsed() const;
    unsigned long getProgramCounter() const;

    // Encapsulation point: all state transitions are validated here,
    // never by external callers touching a public field.
    void setState(ProcessState newState);
    void addCpuTime(unsigned long ticks);
    void advanceProgramCounter(unsigned long instructions);

    std::string toString() const;

private:
    int pid_;
    std::string name_;
    int priority_;
    ProcessState state_;
    unsigned long programCounter_;
    unsigned long cpuTimeUsed_;
};

#endif // MINIOS_PCB_H
