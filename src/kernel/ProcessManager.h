#ifndef MINIOS_PROCESS_MANAGER_H
#define MINIOS_PROCESS_MANAGER_H

#include <memory>
#include <string>
#include <vector>
#include "kernel/KernelModule.h"
#include "process/Process.h"
#include "process/UserProcess.h"
#include "process/SystemProcess.h"
#include "scheduler/Scheduler.h"
#include "report/JsonExportable.h"

// Owns the process table and schedules processes on the (virtual) CPU.
class ProcessManager : public KernelModule, public JsonExportable {
public:
    ProcessManager();
    ~ProcessManager() override;

    void initialize() override;
    void shutdown() override;

    int createUserProcess(const std::string& name, int priority);
    int createSystemProcess(const std::string& name, int priority, const std::string& task);

    Process* findProcess(int pid) const;
    bool hasRunnable() const;

    // Installs a scheduling strategy (FCFS, SJF, Priority, Round Robin).
    void setScheduler(std::unique_ptr<Scheduler> scheduler);

    // Runs one scheduling slice of the process chosen by the scheduler.
    void scheduleOne();
    void terminateAll();

    void listProcesses() const;
    std::size_t processCount() const;

    std::string toJson() const override;

private:
    std::vector<std::unique_ptr<Process>> processes_; // composition + polymorphism
    std::unique_ptr<Scheduler> scheduler_;              // strategy: interchangeable algorithm
    int nextPid_;
};

#endif // MINIOS_PROCESS_MANAGER_H
