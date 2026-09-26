#ifndef MINIOS_FIFO_PAGE_REPLACEMENT_H
#define MINIOS_FIFO_PAGE_REPLACEMENT_H

#include <list>
#include "memory/PageReplacementAlgorithm.h"

// First-In-First-Out: evict the page that was loaded earliest.
class FIFOPageReplacement : public PageReplacementAlgorithm {
public:
    std::string name() const override { return "FIFO"; }

    void onFrameAllocated(int frame, int) override {
        loadOrder_.push_back(frame);
    }

    void onPageAccess(int, int) override {
        // FIFO ignores subsequent references.
    }

    int pickVictim() override {
        int victim = loadOrder_.front();
        loadOrder_.pop_front();
        return victim;
    }

private:
    std::list<int> loadOrder_;
};

#endif // MINIOS_FIFO_PAGE_REPLACEMENT_H
