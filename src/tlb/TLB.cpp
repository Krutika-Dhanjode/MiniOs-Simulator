#include "tlb/TLB.h"
#include <iostream>
#include <sstream>

TLB::TLB(int size) : size_(size), hits_(0), misses_(0) {
    entries_.resize(size, {-1, -1});
}

TLB::~TLB() = default;

void TLB::initialize() {
    std::cout << "[TLB] " << size_ << " entries available\n";
}

void TLB::shutdown() {
    std::cout << "[TLB] shutting down, hits: " << hits_ << ", misses: " << misses_ << "\n";
}

bool TLB::lookup(int page) {
    for (const auto& e : entries_) {
        if (e.first == page) {
            ++hits_;
            return true;
        }
    }
    ++misses_;
    return false;
}

void TLB::load(int page, int frame) {
    // Find an empty slot or evict the oldest (front of vector)
    for (std::size_t i = 0; i < entries_.size(); ++i) {
        if (entries_[i].first == -1) {
            entries_[i] = {page, frame};
            return;
        }
    }
    // FIFO eviction: shift left and insert at back
    for (std::size_t i = 0; i + 1 < entries_.size(); ++i) {
        entries_[i] = entries_[i + 1];
    }
    entries_.back() = {page, frame};
}

void TLB::flush() {
    for (auto& e : entries_) e = {-1, -1};
}

int TLB::getHits() const { return hits_; }
int TLB::getMisses() const { return misses_; }

void TLB::printStats() const {
    int total = hits_ + misses_;
    double rate = total > 0 ? (100.0 * hits_ / total) : 0.0;
    std::cout << "TLB Hits: " << hits_ << " | Misses: " << misses_
              << " | hit rate: " << rate << "%\n";
}

std::string TLB::toJson() const {
    std::ostringstream oss;
    oss << "{\n  \"entries\": [\n";
    for (std::size_t i = 0; i < entries_.size(); ++i) {
        oss << "    {\"slot\": " << i
            << ", \"page\": " << entries_[i].first
            << ", \"frame\": " << entries_[i].second << "}";
        if (i + 1 < entries_.size()) oss << ",";
        oss << "\n";
    }
    oss << "  ],\n  \"hits\": " << hits_ << ",\n  \"misses\": " << misses_ << "\n}";
    return oss.str();
}
