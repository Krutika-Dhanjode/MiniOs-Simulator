#ifndef MINIOS_KERNEL_H
#define MINIOS_KERNEL_H

#include <memory>
#include <vector>
#include "kernel/KernelModule.h"
#include "kernel/ProcessManager.h"
#include "memory/MemoryManager.h"
#include "scheduler/Scheduler.h"
#include "disk/Disk.h"
#include "banker/Banker.h"
#include "report/JsonExporter.h"
#include "tlb/TLB.h"
#include "sync/Semaphore.h"

class PageReplacementAlgorithm; // fwd decl for runPageReplacementDemo
class DiskScheduler;            // fwd decl for runDiskSchedulingDemo

// The kernel: composes all subsystems and drives the boot/run/shutdown loop.
class Kernel {
public:
    Kernel();
    ~Kernel();

    void boot();
    void run();
    void shutdown();
    void exportReport();

    bool isRunning() const;

    // Interactive module menus (called from the main menu loop)
    void processManagementMenu();
    void cpuSchedulingMenu();
    void memoryManagementMenu();
    void pagingMenu();
    void tlbMenu();
    void diskSchedulingMenu();
    void deadlockMenu();
    void synchronizationMenu();

private:
    void registerModule(std::unique_ptr<KernelModule> module);
    void runSchedulerDemo(std::unique_ptr<Scheduler> scheduler);
    void runPageReplacementDemo(std::unique_ptr<PageReplacementAlgorithm> strategy,
                                const std::vector<int>& refString);
    void runDiskSchedulingDemo(std::unique_ptr<DiskScheduler> scheduler);
    void runBankerDemo();

    std::vector<std::unique_ptr<KernelModule>> modules_; // composition + polymorphism
    ProcessManager* processManager_; // non-owning; owned by modules_
    MemoryManager* memoryManager_;   // non-owning; owned by modules_
    Disk* disk_;                     // non-owning; owned by modules_
    Banker* banker_;                 // non-owning; owned by modules_
    TLB* tlb_;                       // non-owning; owned by modules_
    Semaphore* semaphore_;           // non-owning; owned by modules_
    JsonExporter exporter_;          // aggregates JSON from all subsystems
    bool running_;
};

#endif // MINIOS_KERNEL_H
