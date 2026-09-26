#ifndef MINIOS_BANKER_H
#define MINIOS_BANKER_H

#include <vector>
#include "kernel/KernelModule.h"
#include "report/JsonExportable.h"

// Implements the Banker's Algorithm for deadlock avoidance.
// Manages resource allocation, max demand, and need matrices.
class Banker : public KernelModule, public JsonExportable {
public:
    Banker(int numProcesses, int numResources,
           const std::vector<int>& totalResources,
           const std::vector<std::vector<int>>& maxDemand,
           const std::vector<std::vector<int>>& allocation);
    ~Banker() override;

    void initialize() override;
    void shutdown() override;

    // Attempts to grant a resource request. Returns true if granted
    // (leads to a safe state), false otherwise.
    bool requestResources(int pid, const std::vector<int>& request);

    // Checks whether the current state is safe.
    bool isSafeState() const;

    void printState() const;
    void printSafeSequence() const;

    std::string toJson() const override;

private:
    int numProcesses_;
    int numResources_;
    std::vector<int> available_;
    std::vector<std::vector<int>> max_;
    std::vector<std::vector<int>> allocation_;
    std::vector<std::vector<int>> need_;
};

#endif // MINIOS_BANKER_H
