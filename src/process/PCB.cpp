#include "process/PCB.h"
#include <sstream>

PCB::PCB(int pid, const std::string& name, int priority)
    : pid_(pid), name_(name), priority_(priority),
      state_(ProcessState::NEW), programCounter_(0), cpuTimeUsed_(0) {}

int PCB::getPid() const { return pid_; }
const std::string& PCB::getName() const { return name_; }
int PCB::getPriority() const { return priority_; }
ProcessState PCB::getState() const { return state_; }
unsigned long PCB::getCpuTimeUsed() const { return cpuTimeUsed_; }
unsigned long PCB::getProgramCounter() const { return programCounter_; }

void PCB::setState(ProcessState newState) {
    // A terminated process may not transition back to any other state.
    if (state_ == ProcessState::TERMINATED) return;
    state_ = newState;
}

void PCB::addCpuTime(unsigned long ticks) { cpuTimeUsed_ += ticks; }
void PCB::advanceProgramCounter(unsigned long instructions) { programCounter_ += instructions; }

std::string PCB::toString() const {
    std::ostringstream oss;
    oss << "PID " << pid_
        << " | " << name_
        << " | prio " << priority_
        << " | " << ::toString(state_)
        << " | cpu=" << cpuTimeUsed_
        << " | pc=" << programCounter_;
    return oss.str();
}
