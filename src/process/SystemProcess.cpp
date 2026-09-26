#include "process/SystemProcess.h"
#include <iostream>

SystemProcess::SystemProcess(int pid, const std::string& name, int priority, const std::string& task)
    : Process(pid, name, priority), tasks_(2), task_(task) {
    setTotalWork(tasks_);
}

void SystemProcess::run() {
    if (tasks_ > 0) {
        std::cout << "  [kernel] PID " << getPid() << " '" << getName()
                  << "' running task: " << task_ << "\n";
        addCpuTime(1);
        advanceProgramCounter(2);
        --tasks_;
    } else {
        pcb().setState(ProcessState::TERMINATED);
        std::cout << "  [kernel] PID " << getPid() << " '" << getName()
                  << "' task done, exiting.\n";
    }
}
