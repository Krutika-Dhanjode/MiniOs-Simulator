#ifndef MINIOS_LRU_PAGE_REPLACEMENT_H
#define MINIOS_LRU_PAGE_REPLACEMENT_H

#include <map>
#include "memory/PageReplacementAlgorithm.h"

// Least Recently Used: evict the frame whose page was referenced longest ago.
class LRUPageReplacement : public PageReplacementAlgorithm {
public:
    std::string name() const override { return "LRU"; }

    void onFrameAllocated(int frame, int) override {
        lastUsed_[frame] = ++clock_;
    }

    void onPageAccess(int frame, int) override {
        lastUsed_[frame] = ++clock_;
    }

    int pickVictim() override {
        int victim = -1;
        bool first = true;
        long oldest = 0;
        for (const auto& kv : lastUsed_) {
            if (first || kv.second < oldest) {
                oldest = kv.second;
                victim = kv.first;
                first = false;
            }
        }
        lastUsed_.erase(victim);
        return victim;
    }

private:
    std::map<int, long> lastUsed_;
    long clock_ = 0;
};

#endif // MINIOS_LRU_PAGE_REPLACEMENT_H
