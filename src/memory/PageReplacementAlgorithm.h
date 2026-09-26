#ifndef MINIOS_PAGE_REPLACEMENT_ALGORITHM_H
#define MINIOS_PAGE_REPLACEMENT_ALGORITHM_H

#include <string>

// Abstract page replacement strategy.
class PageReplacementAlgorithm {
public:
    virtual ~PageReplacementAlgorithm() = default;

    virtual std::string name() const = 0;

    // Called when a frame is (re)allocated to a page.
    virtual void onFrameAllocated(int frame, int page) = 0;

    // Called on every reference to the page held in a frame (hit or load).
    virtual void onPageAccess(int frame, int page) = 0;

    // Returns the frame number to evict.
    virtual int pickVictim() = 0;
};

#endif // MINIOS_PAGE_REPLACEMENT_ALGORITHM_H
