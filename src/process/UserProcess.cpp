#include "process/UserProcess.h"
#include <iostream>

UserProcess::UserProcess(int pid, const std::string& name, int priority)
    : Process(pid, name, priority), workUnits_(3) {
    setTotalWork(workUnits_);
}

void UserProcess::run() {
    if (workUnits_ > 0) {
        std::cout << "  [user] PID " << getPid() << " '" << getName()
                  << "' executing instruction, work left: " << workUnits_ << "\n";
        addCpuTime(1);
        advanceProgramCounter(4);
        --workUnits_;
    } else {
        pcb().setState(ProcessState::TERMINATED);
        std::cout << "  [user] PID " << getPid() << " '" << getName()
                  << "' completed and exited.\n";
    }
}
