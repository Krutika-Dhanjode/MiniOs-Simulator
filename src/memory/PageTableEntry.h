#ifndef MINIOS_PAGE_TABLE_ENTRY_H
#define MINIOS_PAGE_TABLE_ENTRY_H

// One entry of a page table: maps a virtual page to a physical frame.
class PageTableEntry {
public:
    PageTableEntry() = default;

    bool isValid() const { return valid_; }
    int getFrame() const { return frame_; }

    void map(int frame) {
        frame_ = frame;
        valid_ = true;
    }

    void unmap() {
        frame_ = -1;
        valid_ = false;
    }

private:
    bool valid_ = false;
    int frame_ = -1;
};

#endif // MINIOS_PAGE_TABLE_ENTRY_H
