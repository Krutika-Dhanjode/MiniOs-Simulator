#include "process/Process.h"

Process::Process(int pid, const std::string& name, int priority)
    : pcb_(pid, name, priority) {}

int Process::getPid() const { return pcb_.getPid(); }
const std::string& Process::getName() const { return pcb_.getName(); }
ProcessState Process::getState() const { return pcb_.getState(); }
int Process::getPriority() const { return pcb_.getPriority(); }
const PCB& Process::getPcb() const { return pcb_; }

PCB& Process::pcb() { return pcb_; }

void Process::addCpuTime(unsigned long ticks) { pcb_.addCpuTime(ticks); }
void Process::advanceProgramCounter(unsigned long instructions) { pcb_.advanceProgramCounter(instructions); }

void Process::execute() {
    pcb_.setState(ProcessState::RUNNING);
    run();
    // If the derived run() did not terminate the process, it waits.
    if (pcb_.getState() == ProcessState::RUNNING) {
        pcb_.setState(ProcessState::WAITING);
    }
}

void Process::terminate() {
    pcb_.setState(ProcessState::TERMINATED);
}
