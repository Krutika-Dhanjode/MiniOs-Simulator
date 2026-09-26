#ifndef MINIOS_CSCAN_DISK_SCHEDULER_H
#define MINIOS_CSCAN_DISK_SCHEDULER_H

#include <algorithm>
#include <climits>
#include "disk/DiskScheduler.h"

// C-SCAN (Circular SCAN): sweep only toward higher cylinders; when no
// more requests ahead, wrap around to the lowest request.
class CSCANDiskScheduler : public DiskScheduler {
public:
    std::string name() const override { return "C-SCAN"; }

    int pickNext(const std::vector<int>& queue, int head) override {
        int best = -1;
        int bestDist = INT_MAX;
        for (int r : queue) {
            if (r >= head) {
                int d = r - head;
                if (d < bestDist) { bestDist = d; best = r; }
            }
        }
        if (best < 0) {
            best = *std::min_element(queue.begin(), queue.end());
        }
        return best;
    }
};

#endif // MINIOS_CSCAN_DISK_SCHEDULER_H
