#ifndef MINIOS_TLB_H
#define MINIOS_TLB_H

#include <vector>
#include "kernel/KernelModule.h"
#include "report/JsonExportable.h"

// Translation Lookaside Buffer: a small cache mapping virtual pages to
// physical frames. Uses FIFO replacement when full.
class TLB : public KernelModule, public JsonExportable {
public:
    explicit TLB(int size);
    ~TLB() override;

    void initialize() override;
    void shutdown() override;

    // Returns true if the page is in the TLB (hit), false otherwise (miss).
    // On a miss, the caller should call load() after a page table walk.
    bool lookup(int page);

    // Inserts a page→frame mapping, evicting the oldest entry if full.
    void load(int page, int frame);

    void flush();

    int getHits() const;
    int getMisses() const;
    void printStats() const;
    std::string toJson() const override;

private:
    int size_;
    std::vector<std::pair<int, int>> entries_; // (page, frame), page = -1 means empty
    int hits_;
    int misses_;
};

#endif // MINIOS_TLB_H
