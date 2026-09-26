#include "disk/Disk.h"
#include <iostream>
#include <sstream>

Disk::Disk(int numCylinders)
    : numCylinders_(numCylinders), headPosition_(50), totalSeek_(0) {}

Disk::~Disk() = default;

void Disk::initialize() {
    std::cout << "[Disk] " << numCylinders_ << " cylinders, head at "
              << headPosition_ << "\n";
}

void Disk::shutdown() {
    std::cout << "[Disk] shutting down, total seek distance: "
              << totalSeek_ << " cylinders\n";
}

void Disk::setScheduler(std::unique_ptr<DiskScheduler> scheduler) {
    scheduler_ = std::move(scheduler);
    std::cout << "[Disk] installed " << scheduler_->name()
              << " scheduling policy\n";
}

void Disk::addRequest(int cylinder) {
    queue_.push_back(cylinder);
}

void Disk::run() {
    while (!queue_.empty()) {
        int target = scheduler_->pickNext(queue_, headPosition_);
        int seek = std::abs(target - headPosition_);
        totalSeek_ += seek;
        headPosition_ = target;

        // Remove the serviced request from the queue
        for (auto it = queue_.begin(); it != queue_.end(); ++it) {
            if (*it == target) {
                queue_.erase(it);
                break;
            }
        }

        std::cout << "  [Disk] head " << headPosition_ << " (seek " << seek << ")\n";
    }
}

int Disk::getTotalSeek() const { return totalSeek_; }
int Disk::getCurrentHead() const { return headPosition_; }
int Disk::getQueueLength() const { return static_cast<int>(queue_.size()); }

void Disk::printStats() const {
    std::cout << "Total seek distance: " << totalSeek_ << " cylinders\n";
}

std::string Disk::toJson() const {
    std::ostringstream oss;
    oss << "{\n  \"headPosition\": " << headPosition_ << ",\n";
    oss << "  \"totalSeek\": " << totalSeek_ << ",\n";
    oss << "  \"queue\": [";
    for (std::size_t i = 0; i < queue_.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << queue_[i];
    }
    oss << "]\n}";
    return oss.str();
}
