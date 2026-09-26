#ifndef MINIOS_SYSTEM_PROCESS_H
#define MINIOS_SYSTEM_PROCESS_H

#include "process/Process.h"

// Concrete process type: a privileged kernel task (e.g. I/O completion).
class SystemProcess : public Process {
public:
    SystemProcess(int pid, const std::string& name, int priority, const std::string& task);

protected:
    void run() override;

private:
    unsigned long tasks_;
    std::string task_;
};

#endif // MINIOS_SYSTEM_PROCESS_H
