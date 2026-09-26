#ifndef MINIOS_SEMAPHORE_H
#define MINIOS_SEMAPHORE_H

#include <vector>
#include "kernel/KernelModule.h"
#include "report/JsonExportable.h"

// Counting semaphore with a waiting queue for blocked processes.
class Semaphore : public KernelModule, public JsonExportable {
public:
    explicit Semaphore(int initial);
    ~Semaphore() override;

    void initialize() override;
    void shutdown() override;

    // Blocks the process if the semaphore value is zero.
    void wait(int pid);

    // Releases the semaphore; wakes the next waiting process if any.
    void signal();

    int getValue() const;
    const std::vector<int>& getWaitingQueue() const;
    void printState() const;
    std::string toJson() const override;

private:
    int value_;
    std::vector<int> waitingQueue_;
};

#endif // MINIOS_SEMAPHORE_H
