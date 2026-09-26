#include "sync/Semaphore.h"
#include <iostream>
#include <sstream>

Semaphore::Semaphore(int initial) : value_(initial) {}

Semaphore::~Semaphore() = default;

void Semaphore::initialize() {
    std::cout << "[Semaphore] initialized with value " << value_ << "\n";
}

void Semaphore::shutdown() {
    std::cout << "[Semaphore] shutting down, final value: " << value_ << "\n";
}

void Semaphore::wait(int pid) {
    if (value_ > 0) {
        --value_;
        std::cout << "  [Sync] P" << pid << " acquired semaphore (value=" << value_ << ")\n";
    } else {
        waitingQueue_.push_back(pid);
        std::cout << "  [Sync] P" << pid << " blocked — added to waiting queue\n";
    }
}

void Semaphore::signal() {
    if (!waitingQueue_.empty()) {
        int next = waitingQueue_.front();
        waitingQueue_.erase(waitingQueue_.begin());
        std::cout << "  [Sync] semaphore signaled — P" << next << " woken (value=" << value_ << ")\n";
    } else {
        ++value_;
        std::cout << "  [Sync] semaphore signaled (value=" << value_ << ")\n";
    }
}

int Semaphore::getValue() const { return value_; }
const std::vector<int>& Semaphore::getWaitingQueue() const { return waitingQueue_; }

void Semaphore::printState() const {
    std::cout << "Semaphore value: " << value_ << " | Waiting queue: [";
    for (std::size_t i = 0; i < waitingQueue_.size(); ++i) {
        if (i > 0) std::cout << " ";
        std::cout << "P" << waitingQueue_[i];
    }
    std::cout << "]\n";
}

std::string Semaphore::toJson() const {
    std::ostringstream oss;
    oss << "{\n  \"value\": " << value_ << ",\n  \"waitingQueue\": [";
    for (std::size_t i = 0; i < waitingQueue_.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << waitingQueue_[i];
    }
    oss << "]\n}";
    return oss.str();
}
