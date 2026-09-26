#include "kernel/Kernel.h"
#include "scheduler/FCFSScheduler.h"
#include "scheduler/SJFScheduler.h"
#include "scheduler/PriorityScheduler.h"
#include "scheduler/RoundRobinScheduler.h"
#include "memory/FIFOPageReplacement.h"
#include "memory/LRUPageReplacement.h"
#include "memory/MemoryManager.h"
#include "memory/OptimalPageReplacement.h"
#include "disk/FCFSDiskScheduler.h"
#include "disk/SSTFDiskScheduler.h"
#include "disk/SCANDiskScheduler.h"
#include "disk/CSCANDiskScheduler.h"
#include "banker/Banker.h"
#include "tlb/TLB.h"
#include "sync/Semaphore.h"
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

Kernel::Kernel() : processManager_(nullptr), memoryManager_(nullptr),
                    disk_(nullptr), banker_(nullptr), tlb_(nullptr),
                    semaphore_(nullptr), running_(false) {}

Kernel::~Kernel() {
    if (running_) shutdown();
}

void Kernel::registerModule(std::unique_ptr<KernelModule> module) {
    modules_.push_back(std::move(module));
}

void Kernel::boot() {
    std::cout << "MiniOS v1.0 - booting kernel...\n";

    auto procMod = std::make_unique<ProcessManager>();
    processManager_ = procMod.get();
    registerModule(std::move(procMod));

    auto memMod = std::make_unique<MemoryManager>(3);
    memoryManager_ = memMod.get();
    registerModule(std::move(memMod));

    auto diskMod = std::make_unique<Disk>(200);
    disk_ = diskMod.get();
    registerModule(std::move(diskMod));

    // Classic Banker's algorithm example (Silberschatz):
    // 5 processes, 3 resource types, total = {10, 5, 7}
    std::vector<int> totalRes = {10, 5, 7};
    std::vector<std::vector<int>> maxDemand = {
        {7,5,3}, {3,2,2}, {9,0,2}, {2,2,2}, {4,3,3}
    };
    std::vector<std::vector<int>> alloc = {
        {0,1,0}, {2,0,0}, {3,0,2}, {2,1,1}, {0,0,2}
    };
    auto bankerMod = std::make_unique<Banker>(5, 3, totalRes, maxDemand, alloc);
    banker_ = bankerMod.get();
    registerModule(std::move(bankerMod));

    auto tlbMod = std::make_unique<TLB>(4);
    tlb_ = tlbMod.get();
    registerModule(std::move(tlbMod));

    auto semMod = std::make_unique<Semaphore>(1);
    semaphore_ = semMod.get();
    registerModule(std::move(semMod));

    for (auto& m : modules_) m->initialize(); // virtual dispatch
    running_ = true;
}

void Kernel::run() {
    if (!running_) return;
    std::cout << "\n--- CPU Scheduling Algorithms ---\n";
    runSchedulerDemo(std::make_unique<FCFSScheduler>());
    runSchedulerDemo(std::make_unique<SJFScheduler>());
    runSchedulerDemo(std::make_unique<PriorityScheduler>());
    runSchedulerDemo(std::make_unique<RoundRobinScheduler>());

    std::cout << "\n--- Page replacement algorithms ---\n";
    const std::vector<int> refString = {0, 1, 2, 3, 0, 1, 4, 0, 1, 2, 3, 4};
    runPageReplacementDemo(std::make_unique<FIFOPageReplacement>(), refString);
    runPageReplacementDemo(std::make_unique<LRUPageReplacement>(), refString);
    runPageReplacementDemo(std::make_unique<OptimalPageReplacement>(refString), refString);

    std::cout << "\n--- Disk scheduling algorithms ---\n";
    runDiskSchedulingDemo(std::make_unique<FCFSDiskScheduler>());
    runDiskSchedulingDemo(std::make_unique<SSTFDiskScheduler>());
    runDiskSchedulingDemo(std::make_unique<SCANDiskScheduler>());
    runDiskSchedulingDemo(std::make_unique<CSCANDiskScheduler>());

    std::cout << "\n--- Banker's algorithm ---\n";
    runBankerDemo();

    exportReport();
}

void Kernel::runSchedulerDemo(std::unique_ptr<Scheduler> scheduler) {
    const std::string name = scheduler->name();
    std::cout << "\n=== " << name << " ===\n";
    ProcessManager pm;
    pm.setScheduler(std::move(scheduler));
    pm.createUserProcess("shell", 5);
    pm.createUserProcess("editor", 3);
    pm.createSystemProcess("iod", 9, "disk I/O completion");

    const int maxSlices = 12;
    for (int i = 0; i < maxSlices && pm.hasRunnable(); ++i) {
        pm.scheduleOne();
    }

    std::cout << "--- Final states (" << name << ") ---\n";
    pm.listProcesses();
    exporter_.addSection("scheduling_" + name, pm);
}

void Kernel::runPageReplacementDemo(std::unique_ptr<PageReplacementAlgorithm> strategy,
                                    const std::vector<int>& refString) {
    const std::string name = strategy->name();
    const int numFrames = 3;
    std::cout << "\n=== " << name << " ===\n";

    MemoryManager mm(numFrames);
    mm.setStrategy(std::move(strategy));
    mm.loadProcess(1, 5); // PID 1 with 5 pages (0..4)

    std::cout << "Reference string: ";
    for (int page : refString) std::cout << page << " ";
    std::cout << "\nPhysical frames: " << numFrames << "\n";

    for (int page : refString) {
        bool hit = mm.accessPage(1, page);
        std::cout << "  access page " << page << " -> " << (hit ? "HIT" : "FAULT") << "\n";
    }

    mm.dumpFrames();
    mm.printStats();
    exporter_.addSection("paging_" + name, mm);
}

void Kernel::runDiskSchedulingDemo(std::unique_ptr<DiskScheduler> scheduler) {
    const std::string name = scheduler->name();
    std::cout << "\n=== " << name << " ===\n";

    Disk disk(200);
    disk.setScheduler(std::move(scheduler));

    // Classic request queue, head starts at cylinder 50
    std::vector<int> requests = {98, 183, 37, 122, 14, 124, 65, 67};
    std::cout << "Request queue: ";
    for (int r : requests) std::cout << r << " ";
    std::cout << "\nInitial head position: 50\n";

    for (int r : requests) disk.addRequest(r);
    disk.run();
    disk.printStats();
    exporter_.addSection("disk_" + name, disk);
}

void Kernel::runBankerDemo() {
    std::cout << "\n=== Banker's Algorithm ===\n";

    // Classic Silberschatz example: 5 processes, 3 resource types
    Banker banker(5, 3, {10, 5, 7},
                   {{7,5,3}, {3,2,2}, {9,0,2}, {2,2,2}, {4,3,3}},  // max
                   {{0,1,0}, {2,0,0}, {3,0,2}, {2,1,1}, {0,0,2}}); // allocation

    banker.printState();
    banker.printSafeSequence();

    // P1 requests {1, 0, 2} — should be granted (safe)
    std::cout << "\nP1 requests {1, 0, 2}:\n";
    banker.requestResources(1, {1, 0, 2});
    banker.printState();

    // P4 requests {3, 3, 0} — should be denied (exceeds available)
    std::cout << "\nP4 requests {3, 3, 0}:\n";
    banker.requestResources(4, {3, 3, 0});

    // P0 requests {0, 2, 0} — should be granted (safe)
    std::cout << "\nP0 requests {0, 2, 0}:\n";
    banker.requestResources(0, {0, 2, 0});
    banker.printState();
    exporter_.addSection("banker", banker);
}

void Kernel::exportReport() {
    std::cout << "\n--- Exporting system report ---\n";
    if (exporter_.writeToFile("minios_report.json")) {
        std::cout << "[Kernel] Report written to minios_report.json\n";
    } else {
        std::cout << "[Kernel] Failed to write report file\n";
    }
}

void Kernel::shutdown() {
    std::cout << "\n--- Kernel shutdown ---\n";
    processManager_->terminateAll();
    for (auto& m : modules_) m->shutdown();
    running_ = false;
}

bool Kernel::isRunning() const { return running_; }

// ---------------------------------------------------------------------------
// Interactive module menus
// ---------------------------------------------------------------------------

static int readInt(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) return value;
        if (std::cin.eof()) std::exit(0);
        std::cout << "Invalid input — please enter a number.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

static std::string readString(const std::string& prompt) {
    std::string value;
    std::cout << prompt;
    if (!(std::cin >> value)) std::exit(0);
    return value;
}

static std::vector<int> readIntList(const std::string& prompt) {
    std::vector<int> values;
    std::cout << prompt;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::string line;
    if (!std::getline(std::cin, line)) std::exit(0);
    std::istringstream iss(line);
    int v;
    while (iss >> v) values.push_back(v);
    return values;
}

void Kernel::processManagementMenu() {
    std::cout << "\n--- Process Management ---\n";
    ProcessManager pm;
    int count = readInt("How many processes to create? ");

    for (int i = 0; i < count; ++i) {
        std::cout << "\nProcess " << (i + 1) << ":\n";
        std::string name = readString("  Name: ");
        int prio = readInt("  Priority (lower = higher): ");
        int type = readInt("  Type (1 = user, 2 = system): ");
        if (type == 2) {
            std::string task = readString("  Kernel task description: ");
            pm.createSystemProcess(name, prio, task);
        } else {
            pm.createUserProcess(name, prio);
        }
    }

    std::cout << "\nCreated processes:\n";
    pm.listProcesses();

    char sched = readString("Schedule them now? (y/n): ")[0];
    if (sched == 'y' || sched == 'Y') {
        int algo = readInt("Algorithm (1=FCFS, 2=SJF, 3=Priority, 4=Round Robin): ");
        switch (algo) {
            case 1: pm.setScheduler(std::make_unique<FCFSScheduler>()); break;
            case 2: pm.setScheduler(std::make_unique<SJFScheduler>()); break;
            case 3: pm.setScheduler(std::make_unique<PriorityScheduler>()); break;
            default: pm.setScheduler(std::make_unique<RoundRobinScheduler>()); break;
        }
        int slices = readInt("How many scheduling slices? ");
        for (int i = 0; i < slices && pm.hasRunnable(); ++i) pm.scheduleOne();
        std::cout << "\nFinal states:\n";
        pm.listProcesses();
    }
    exporter_.addSection("process_management", pm);
}

void Kernel::cpuSchedulingMenu() {
    std::cout << "\n--- CPU Scheduling ---\n";
    int algo = readInt("Algorithm (1=FCFS, 2=SJF, 3=Priority, 4=Round Robin): ");

    ProcessManager pm;
    switch (algo) {
        case 1: pm.setScheduler(std::make_unique<FCFSScheduler>()); break;
        case 2: pm.setScheduler(std::make_unique<SJFScheduler>()); break;
        case 3: pm.setScheduler(std::make_unique<PriorityScheduler>()); break;
        default: pm.setScheduler(std::make_unique<RoundRobinScheduler>()); break;
    }

    int count = readInt("How many processes? ");
    for (int i = 0; i < count; ++i) {
        std::cout << "\nProcess " << (i + 1) << ":\n";
        std::string name = readString("  Name: ");
        int prio = readInt("  Priority: ");
        int type = readInt("  Type (1 = user, 2 = system): ");
        if (type == 2) {
            std::string task = readString("  Kernel task: ");
            pm.createSystemProcess(name, prio, task);
        } else {
            pm.createUserProcess(name, prio);
        }
    }

    int slices = readInt("How many scheduling slices? ");
    for (int i = 0; i < slices && pm.hasRunnable(); ++i) pm.scheduleOne();
    pm.listProcesses();
    exporter_.addSection("cpu_scheduling", pm);
}

void Kernel::memoryManagementMenu() {
    std::cout << "\n--- Memory Management ---\n";
    int frames = readInt("Number of physical frames: ");
    MemoryManager mm(frames);

    std::cout << "Memory layout (" << frames << " frames):\n";
    for (int i = 0; i < frames; ++i) {
        std::cout << "  Frame " << i << ": free\n";
    }
    std::cout << "Frames available: " << frames << "\n";
    mm.printStats();
    exporter_.addSection("memory_management", mm);
}

void Kernel::pagingMenu() {
    std::cout << "\n--- Paging & Page Replacement ---\n";
    std::vector<int> refString = readIntList(
        "Enter reference string (space-separated page numbers): ");
    int frames = readInt("Number of physical frames: ");
    int algo = readInt("Policy (1=FIFO, 2=LRU, 3=Optimal): ");

    MemoryManager mm(frames);
    switch (algo) {
        case 1: mm.setStrategy(std::make_unique<FIFOPageReplacement>()); break;
        case 2: mm.setStrategy(std::make_unique<LRUPageReplacement>()); break;
        default: mm.setStrategy(std::make_unique<OptimalPageReplacement>(refString)); break;
    }

    int pages = 0;
    for (int p : refString) pages = std::max(pages, p + 1);
    mm.loadProcess(1, pages);

    std::cout << "\nReference string: ";
    for (int p : refString) std::cout << p << " ";
    std::cout << "\n";

    for (int page : refString) {
        bool hit = mm.accessPage(1, page);
        std::cout << "  access page " << page << " -> " << (hit ? "HIT" : "FAULT") << "\n";
    }

    mm.dumpFrames();
    mm.printStats();
    exporter_.addSection("paging", mm);
}

void Kernel::tlbMenu() {
    std::cout << "\n--- TLB Simulation ---\n";
    int size = readInt("TLB size (number of entries): ");
    std::vector<int> refString = readIntList(
        "Enter reference string (space-separated page numbers): ");

    TLB tlb(size);

    std::cout << "\nTLB simulation (" << size << " entries):\n";
    for (int page : refString) {
        bool hit = tlb.lookup(page);
        std::cout << "  access page " << page << " -> " << (hit ? "HIT" : "MISS") << "\n";
        if (!hit) tlb.load(page, page % 10); // simulate page table walk
    }

    tlb.printStats();
    exporter_.addSection("tlb", tlb);
}

void Kernel::diskSchedulingMenu() {
    std::cout << "\n--- Disk Scheduling ---\n";
    std::vector<int> requests = readIntList(
        "Enter request queue (space-separated cylinder numbers): ");
    int head = readInt("Initial head position: ");
    int algo = readInt("Algorithm (1=FCFS, 2=SSTF, 3=SCAN, 4=C-SCAN): ");

    Disk disk(200);
    switch (algo) {
        case 1: disk.setScheduler(std::make_unique<FCFSDiskScheduler>()); break;
        case 2: disk.setScheduler(std::make_unique<SSTFDiskScheduler>()); break;
        case 3: disk.setScheduler(std::make_unique<SCANDiskScheduler>()); break;
        default: disk.setScheduler(std::make_unique<CSCANDiskScheduler>()); break;
    }

    std::cout << "\nRequest queue: ";
    for (int r : requests) std::cout << r << " ";
    std::cout << "\nInitial head: " << head << "\n";

    for (int r : requests) disk.addRequest(r);
    disk.run();
    disk.printStats();
    exporter_.addSection("disk_scheduling", disk);
}

void Kernel::deadlockMenu() {
    std::cout << "\n--- Deadlock Management (Banker's Algorithm) ---\n";
    int numProcs = readInt("Number of processes: ");
    int numRes = readInt("Number of resource types: ");

    std::vector<int> total(numRes);
    std::cout << "Enter total instances for each resource type:\n";
    for (int j = 0; j < numRes; ++j) {
        total[j] = readInt("  Resource " + std::string(1, 'A' + j) + ": ");
    }

    std::vector<std::vector<int>> maxDemand(numProcs, std::vector<int>(numRes));
    std::vector<std::vector<int>> allocation(numProcs, std::vector<int>(numRes));

    for (int i = 0; i < numProcs; ++i) {
        std::cout << "\nP" << i << " max demand: ";
        for (int j = 0; j < numRes; ++j) maxDemand[i][j] = readInt("");
        std::cout << "P" << i << " allocation: ";
        for (int j = 0; j < numRes; ++j) allocation[i][j] = readInt("");
    }

    Banker banker(numProcs, numRes, total, maxDemand, allocation);
    banker.printState();
    banker.printSafeSequence();

    char req = readString("Test a resource request? (y/n): ")[0];
    if (req == 'y' || req == 'Y') {
        int pid = readInt("  Process ID: ");
        std::vector<int> request(numRes);
        std::cout << "  Request: ";
        for (int j = 0; j < numRes; ++j) request[j] = readInt("");
        banker.requestResources(pid, request);
        banker.printState();
    }
    exporter_.addSection("banker", banker);
}

void Kernel::synchronizationMenu() {
    std::cout << "\n--- Synchronization ---\n";
    int initial = readInt("Initial semaphore value: ");
    Semaphore sem(initial);

    int ops = readInt("Number of operations: ");
    for (int i = 0; i < ops; ++i) {
        std::cout << "\nOperation " << (i + 1) << ":\n";
        int op = readInt("  Type (1 = wait, 2 = signal): ");
        if (op == 1) {
            int pid = readInt("  Process ID: ");
            sem.wait(pid);
        } else {
            sem.signal();
        }
        sem.printState();
    }
    exporter_.addSection("synchronization", sem);
}
