#ifndef MINIOS_USER_PROCESS_H
#define MINIOS_USER_PROCESS_H

#include "process/Process.h"

// Concrete process type: a user-space program with a fixed amount of work.
class UserProcess : public Process {
public:
    UserProcess(int pid, const std::string& name, int priority);

protected:
    void run() override;

private:
    unsigned long workUnits_;
};

#endif // MINIOS_USER_PROCESS_H
