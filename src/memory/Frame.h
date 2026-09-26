#ifndef MINIOS_FRAME_H
#define MINIOS_FRAME_H

#include "memory/Page.h"

// A physical memory frame: either free or holding exactly one page.
class Frame {
public:
    explicit Frame(int number) : number_(number) {}

    int getNumber() const { return number_; }
    bool isOccupied() const { return occupied_; }
    const Page& getOccupant() const { return occupant_; }

    void load(const Page& page) {
        occupant_ = page;
        occupied_ = true;
    }

    void evict() {
        occupied_ = false;
        occupant_ = Page();
    }

private:
    int number_;
    bool occupied_ = false;
    Page occupant_;
};

#endif // MINIOS_FRAME_H
