#ifndef MINIOS_DISK_H
#define MINIOS_DISK_H

#include <memory>
#include <vector>
#include "kernel/KernelModule.h"
#include "disk/DiskScheduler.h"
#include "report/JsonExportable.h"

// Simulates a disk drive with a movable read/write head.
// The scheduling policy is a pluggable strategy.
class Disk : public KernelModule, public JsonExportable {
public:
    explicit Disk(int numCylinders);
    ~Disk() override;

    void initialize() override;
    void shutdown() override;

    void setScheduler(std::unique_ptr<DiskScheduler> scheduler);
    void addRequest(int cylinder);

    // Processes all queued requests, one per call.
    void run();

    int getTotalSeek() const;
    int getCurrentHead() const;
    int getQueueLength() const;
    void printStats() const;

    std::string toJson() const override;

private:
    int numCylinders_;
    int headPosition_;
    int totalSeek_;
    std::vector<int> queue_;
    std::unique_ptr<DiskScheduler> scheduler_;
};

#endif // MINIOS_DISK_H
