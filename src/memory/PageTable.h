#ifndef MINIOS_PAGE_TABLE_H
#define MINIOS_PAGE_TABLE_H

#include <vector>
#include "memory/PageTableEntry.h"

// Per-process translation table from virtual pages to physical frames.
class PageTable {
public:
    explicit PageTable(int numPages) : entries_(numPages) {}

    PageTableEntry& entry(int page) { return entries_[page]; }
    const std::vector<PageTableEntry>& entries() const { return entries_; }

    void map(int page, int frame) { entries_[page].map(frame); }
    void unmap(int page) { entries_[page].unmap(); }

private:
    std::vector<PageTableEntry> entries_;
};

#endif // MINIOS_PAGE_TABLE_H
