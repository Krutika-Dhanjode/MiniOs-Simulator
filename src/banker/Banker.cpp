#include "banker/Banker.h"
#include <iostream>
#include <sstream>

Banker::Banker(int numProcesses, int numResources,
               const std::vector<int>& totalResources,
               const std::vector<std::vector<int>>& maxDemand,
               const std::vector<std::vector<int>>& allocation)
    : numProcesses_(numProcesses), numResources_(numResources),
      max_(maxDemand), allocation_(allocation) {
    // Compute available = total - sum(allocation)
    available_ = totalResources;
    for (int i = 0; i < numProcesses_; ++i) {
        for (int j = 0; j < numResources_; ++j) {
            available_[j] -= allocation_[i][j];
        }
    }

    // Compute need = max - allocation
    need_.resize(numProcesses_, std::vector<int>(numResources_));
    for (int i = 0; i < numProcesses_; ++i) {
        for (int j = 0; j < numResources_; ++j) {
            need_[i][j] = max_[i][j] - allocation_[i][j];
        }
    }
}

Banker::~Banker() = default;

void Banker::initialize() {
    std::cout << "[Banker] " << numProcesses_ << " processes, "
              << numResources_ << " resource types\n";
}

void Banker::shutdown() {
    std::cout << "[Banker] shutting down\n";
}

bool Banker::requestResources(int pid, const std::vector<int>& request) {
    // Check request <= need
    for (int j = 0; j < numResources_; ++j) {
        if (request[j] > need_[pid][j]) {
            std::cout << "  [Banker] P" << pid
                      << " request exceeds max claim — denied\n";
            return false;
        }
    }

    // Check request <= available
    for (int j = 0; j < numResources_; ++j) {
        if (request[j] > available_[j]) {
            std::cout << "  [Banker] P" << pid
                      << " must wait — insufficient resources\n";
            return false;
        }
    }

    // Tentative allocation
    for (int j = 0; j < numResources_; ++j) {
        available_[j] -= request[j];
        allocation_[pid][j] += request[j];
        need_[pid][j] -= request[j];
    }

    if (isSafeState()) {
        std::cout << "  [Banker] P" << pid << " request granted — safe state\n";
        return true;
    }

    // Rollback
    for (int j = 0; j < numResources_; ++j) {
        available_[j] += request[j];
        allocation_[pid][j] -= request[j];
        need_[pid][j] += request[j];
    }
    std::cout << "  [Banker] P" << pid
              << " request denied — would lead to unsafe state\n";
    return false;
}

bool Banker::isSafeState() const {
    std::vector<int> work = available_;
    std::vector<bool> finish(numProcesses_, false);

    for (int count = 0; count < numProcesses_; ++count) {
        bool found = false;
        for (int i = 0; i < numProcesses_; ++i) {
            if (finish[i]) continue;
            bool canRun = true;
            for (int j = 0; j < numResources_; ++j) {
                if (need_[i][j] > work[j]) {
                    canRun = false;
                    break;
                }
            }
            if (canRun) {
                for (int j = 0; j < numResources_; ++j) {
                    work[j] += allocation_[i][j];
                }
                finish[i] = true;
                found = true;
                break; // one process per step — sequential semantics
            }
        }
        if (!found) return false;
    }
    return true;
}

void Banker::printState() const {
    std::cout << "Available: [";
    for (int j = 0; j < numResources_; ++j) {
        if (j > 0) std::cout << " ";
        std::cout << available_[j];
    }
    std::cout << "]\n";

    std::cout << "Process | Allocation | Max | Need\n";
    for (int i = 0; i < numProcesses_; ++i) {
        std::cout << "P" << i << "      | ";
        for (int j = 0; j < numResources_; ++j) {
            std::cout << allocation_[i][j] << " ";
        }
        std::cout << "| ";
        for (int j = 0; j < numResources_; ++j) {
            std::cout << max_[i][j] << " ";
        }
        std::cout << "| ";
        for (int j = 0; j < numResources_; ++j) {
            std::cout << need_[i][j] << " ";
        }
        std::cout << "\n";
    }
}

void Banker::printSafeSequence() const {
    std::vector<int> work = available_;
    std::vector<bool> finish(numProcesses_, false);
    std::vector<int> sequence;

    for (int count = 0; count < numProcesses_; ++count) {
        for (int i = 0; i < numProcesses_; ++i) {
            if (finish[i]) continue;
            bool canRun = true;
            for (int j = 0; j < numResources_; ++j) {
                if (need_[i][j] > work[j]) {
                    canRun = false;
                    break;
                }
            }
            if (canRun) {
                for (int j = 0; j < numResources_; ++j) {
                    work[j] += allocation_[i][j];
                }
                finish[i] = true;
                sequence.push_back(i);
            }
        }
    }

    std::cout << "Safe sequence: ";
    for (size_t i = 0; i < sequence.size(); ++i) {
        if (i > 0) std::cout << " -> ";
        std::cout << "P" << sequence[i];
    }
    std::cout << "\n";
}

std::string Banker::toJson() const {
    std::ostringstream oss;
    oss << "{\n  \"available\": [";
    for (int j = 0; j < numResources_; ++j) {
        if (j > 0) oss << ", ";
        oss << available_[j];
    }
    oss << "],\n  \"processes\": [\n";
    for (int i = 0; i < numProcesses_; ++i) {
        oss << "    {\"pid\": " << i << ", \"allocation\": [";
        for (int j = 0; j < numResources_; ++j) {
            if (j > 0) oss << ", ";
            oss << allocation_[i][j];
        }
        oss << "], \"max\": [";
        for (int j = 0; j < numResources_; ++j) {
            if (j > 0) oss << ", ";
            oss << max_[i][j];
        }
        oss << "], \"need\": [";
        for (int j = 0; j < numResources_; ++j) {
            if (j > 0) oss << ", ";
            oss << need_[i][j];
        }
        oss << "]}";
        if (i + 1 < numProcesses_) oss << ",";
        oss << "\n";
    }
    oss << "  ]\n}";
    return oss.str();
}
