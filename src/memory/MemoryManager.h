#ifndef MINIOS_MEMORY_MANAGER_H
#define MINIOS_MEMORY_MANAGER_H

#include <map>
#include <memory>
#include <vector>
#include "kernel/KernelModule.h"
#include "memory/Frame.h"
#include "memory/PageTable.h"
#include "memory/PageReplacementAlgorithm.h"
#include "report/JsonExportable.h"

// Simulates demand paging over a fixed set of physical frames.
// The page replacement policy is a pluggable strategy.
class MemoryManager : public KernelModule, public JsonExportable {
public:
    explicit MemoryManager(int numFrames);
    ~MemoryManager() override;

    void initialize() override;
    void shutdown() override;

    void setStrategy(std::unique_ptr<PageReplacementAlgorithm> strategy);
    void loadProcess(int pid, int numPages);

    // Simulates one page reference; returns true on hit, false on fault.
    bool accessPage(int pid, int page);

    int getHits() const;
    int getFaults() const;
    void resetStats();
    void dumpFrames() const;
    void printStats() const;

    std::string toJson() const override;

private:
    int findFreeFrame() const;
    void handlePageFault(int pid, int page);

    std::vector<Frame> frames_;                          // physical memory
    std::map<int, PageTable> pageTables_;                // one table per process
    std::unique_ptr<PageReplacementAlgorithm> strategy_; // pluggable policy
    int hits_;
    int faults_;
};

#endif // MINIOS_MEMORY_MANAGER_H
