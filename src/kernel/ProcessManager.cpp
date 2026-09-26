#include "kernel/ProcessManager.h"
#include <iostream>
#include <sstream>

ProcessManager::ProcessManager() : nextPid_(1) {}

ProcessManager::~ProcessManager() = default;

void ProcessManager::initialize() {
    std::cout << "[ProcessManager] process table ready, next PID = " << nextPid_ << "\n";
}

void ProcessManager::shutdown() {
    std::cout << "[ProcessManager] shutting down, " << processCount()
              << " processes in table\n";
}

int ProcessManager::createUserProcess(const std::string& name, int priority) {
    auto proc = std::make_unique<UserProcess>(nextPid_++, name, priority);
    int pid = proc->getPid();
    processes_.push_back(std::move(proc));
    std::cout << "[Kernel] created user process '" << name << "' (PID " << pid << ")\n";
    return pid;
}

int ProcessManager::createSystemProcess(const std::string& name, int priority, const std::string& task) {
    auto proc = std::make_unique<SystemProcess>(nextPid_++, name, priority, task);
    int pid = proc->getPid();
    processes_.push_back(std::move(proc));
    std::cout << "[Kernel] created system process '" << name << "' (PID " << pid << ")\n";
    return pid;
}

Process* ProcessManager::findProcess(int pid) const {
    for (const auto& p : processes_) {
        if (p->getPid() == pid) return p.get();
    }
    return nullptr;
}

bool ProcessManager::hasRunnable() const {
    for (const auto& p : processes_) {
        if (p->getState() != ProcessState::TERMINATED) return true;
    }
    return false;
}

void ProcessManager::setScheduler(std::unique_ptr<Scheduler> scheduler) {
    scheduler_ = std::move(scheduler);
    std::cout << "[ProcessManager] installed " << scheduler_->name() << " scheduler\n";
}

void ProcessManager::scheduleOne() {
    Process* next = scheduler_->pickNext(processes_); // polymorphic dispatch
    if (!next) {
        std::cout << "[" << scheduler_->name() << "] no runnable processes\n";
        return;
    }
    std::cout << "[" << scheduler_->name() << "] dispatching PID " << next->getPid() << "\n";
    next->execute(); // virtual dispatch
}

void ProcessManager::terminateAll() {
    for (auto& p : processes_) {
        if (p->getState() != ProcessState::TERMINATED) p->terminate();
    }
}

void ProcessManager::listProcesses() const {
    for (const auto& p : processes_) {
        std::cout << "  " << p->getPcb().toString() << "\n";
    }
}

std::size_t ProcessManager::processCount() const { return processes_.size(); }

std::string ProcessManager::toJson() const {
    std::ostringstream oss;
    oss << "{\n  \"processes\": [\n";
    for (std::size_t i = 0; i < processes_.size(); ++i) {
        const auto& p = processes_[i];
        oss << "    {\"pid\": " << p->getPid()
            << ", \"name\": \"" << p->getName() << "\""
            << ", \"priority\": " << p->getPriority()
            << ", \"state\": \"" << toString(p->getState()) << "\""
            << ", \"cpu\": " << p->getPcb().getCpuTimeUsed()
            << ", \"pc\": " << p->getPcb().getProgramCounter()
            << "}";
        if (i + 1 < processes_.size()) oss << ",";
        oss << "\n";
    }
    oss << "  ]\n}";
    return oss.str();
}
