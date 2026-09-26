#ifndef MINIOS_SCAN_DISK_SCHEDULER_H
#define MINIOS_SCAN_DISK_SCHEDULER_H

#include <climits>
#include "disk/DiskScheduler.h"

// SCAN (elevator): sweep in one direction, servicing requests along the
// way; reverse direction when no more requests ahead.
class SCANDiskScheduler : public DiskScheduler {
public:
    std::string name() const override { return "SCAN"; }

    int pickNext(const std::vector<int>& queue, int head) override {
        int best = -1;
        int bestDist = INT_MAX;
        for (int r : queue) {
            if (direction_ > 0 && r >= head) {
                int d = r - head;
                if (d < bestDist) { bestDist = d; best = r; }
            } else if (direction_ < 0 && r <= head) {
                int d = head - r;
                if (d < bestDist) { bestDist = d; best = r; }
            }
        }
        if (best < 0) {
            direction_ = -direction_;
            return pickNext(queue, head);
        }
        return best;
    }

private:
    int direction_ = 1; // start sweeping toward higher cylinders
};

#endif // MINIOS_SCAN_DISK_SCHEDULER_H
