#include "memory/MemoryManager.h"
#include <iostream>
#include <sstream>

MemoryManager::MemoryManager(int numFrames) : hits_(0), faults_(0) {
    for (int i = 0; i < numFrames; ++i) frames_.emplace_back(i);
}

MemoryManager::~MemoryManager() = default;

void MemoryManager::initialize() {
    std::cout << "[MemoryManager] " << frames_.size()
              << " physical frames available\n";
}

void MemoryManager::shutdown() {
    std::cout << "[MemoryManager] shutting down, faults so far: " << faults_ << "\n";
}

void MemoryManager::setStrategy(std::unique_ptr<PageReplacementAlgorithm> strategy) {
    strategy_ = std::move(strategy);
    std::cout << "[MemoryManager] installed " << strategy_->name()
              << " page replacement policy\n";
}

void MemoryManager::loadProcess(int pid, int numPages) {
    pageTables_.insert_or_assign(pid, PageTable(numPages));
}

bool MemoryManager::accessPage(int pid, int page) {
    auto it = pageTables_.find(pid);
    if (it == pageTables_.end()) {
        std::cerr << "[MemoryManager] unknown PID " << pid << "\n";
        return true;
    }

    PageTableEntry& entry = it->second.entry(page);
    if (entry.isValid()) {
        ++hits_;
        strategy_->onPageAccess(entry.getFrame(), page);
        return true;
    }

    ++faults_;
    handlePageFault(pid, page);
    return false;
}

int MemoryManager::findFreeFrame() const {
    for (const auto& f : frames_) {
        if (!f.isOccupied()) return f.getNumber();
    }
    return -1;
}

void MemoryManager::handlePageFault(int pid, int page) {
    int frame = findFreeFrame();
    if (frame < 0) {
        frame = strategy_->pickVictim();
        Page evicted = frames_[frame].getOccupant();
        pageTables_.at(evicted.getPid()).unmap(evicted.getPageNumber());
    }

    frames_[frame].load(Page(pid, page));
    pageTables_.at(pid).map(page, frame);
    strategy_->onFrameAllocated(frame, page);
    strategy_->onPageAccess(frame, page);
}

int MemoryManager::getHits() const { return hits_; }
int MemoryManager::getFaults() const { return faults_; }

void MemoryManager::resetStats() {
    hits_ = 0;
    faults_ = 0;
}

void MemoryManager::dumpFrames() const {
    std::cout << "Frames: [";
    for (std::size_t i = 0; i < frames_.size(); ++i) {
        if (i > 0) std::cout << " ";
        const Frame& f = frames_[i];
        if (f.isOccupied()) {
            std::cout << "P" << f.getOccupant().getPid()
                      << ":page" << f.getOccupant().getPageNumber();
        } else {
            std::cout << "free";
        }
    }
    std::cout << "]\n";
}

void MemoryManager::printStats() const {
    int total = hits_ + faults_;
    double rate = total > 0 ? (100.0 * faults_ / total) : 0.0;
    std::cout << "Hits: " << hits_ << " | Faults: " << faults_
              << " | fault rate: " << rate << "%\n";
}

std::string MemoryManager::toJson() const {
    std::ostringstream oss;
    oss << "{\n  \"frames\": [\n";
    for (std::size_t i = 0; i < frames_.size(); ++i) {
        const Frame& f = frames_[i];
        oss << "    {\"frame\": " << f.getNumber()
            << ", \"occupied\": " << (f.isOccupied() ? "true" : "false");
        if (f.isOccupied()) {
            oss << ", \"pid\": " << f.getOccupant().getPid()
                << ", \"page\": " << f.getOccupant().getPageNumber();
        }
        oss << "}";
        if (i + 1 < frames_.size()) oss << ",";
        oss << "\n";
    }
    oss << "  ],\n  \"hits\": " << hits_ << ",\n  \"faults\": " << faults_ << "\n}";
    return oss.str();
}
