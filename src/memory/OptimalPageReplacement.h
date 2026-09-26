#ifndef MINIOS_OPTIMAL_PAGE_REPLACEMENT_H
#define MINIOS_OPTIMAL_PAGE_REPLACEMENT_H

#include <climits>
#include <map>
#include <vector>
#include "memory/PageReplacementAlgorithm.h"

// Optimal (Belady's MIN): evict the page whose next use is farthest away.
// Non-causal: requires the full reference string up front.
class OptimalPageReplacement : public PageReplacementAlgorithm {
public:
    OptimalPageReplacement() = default;
    explicit OptimalPageReplacement(const std::vector<int>& referenceString)
        : refs_(referenceString) {}

    void setReferenceString(const std::vector<int>& referenceString) {
        refs_ = referenceString;
        pos_ = 0;
    }

    std::string name() const override { return "Optimal"; }

    void onFrameAllocated(int frame, int page) override {
        framePage_[frame] = page;
    }

    void onPageAccess(int frame, int page) override {
        framePage_[frame] = page;
        // Advance past the current reference; the sequence must match.
        while (pos_ < refs_.size() && refs_[pos_] != page) ++pos_;
        if (pos_ < refs_.size() && refs_[pos_] == page) ++pos_;
    }

    int pickVictim() override {
        int victim = -1;
        int farthest = -1;
        for (const auto& kv : framePage_) {
            int next = nextUse(kv.second);
            if (next > farthest) {
                farthest = next;
                victim = kv.first;
            }
        }
        framePage_.erase(victim);
        return victim;
    }

private:
    int nextUse(int page) const {
        for (std::size_t i = pos_; i < refs_.size(); ++i) {
            if (refs_[i] == page) return static_cast<int>(i);
        }
        return INT_MAX;
    }

    std::vector<int> refs_;
    std::map<int, int> framePage_;
    std::size_t pos_ = 0;
};

#endif // MINIOS_OPTIMAL_PAGE_REPLACEMENT_H
