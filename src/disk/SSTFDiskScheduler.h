#ifndef MINIOS_SSTF_DISK_SCHEDULER_H
#define MINIOS_SSTF_DISK_SCHEDULER_H

#include <cstdlib>
#include "disk/DiskScheduler.h"

// Shortest Seek Time First: service the nearest request.
class SSTFDiskScheduler : public DiskScheduler {
public:
    std::string name() const override { return "SSTF"; }

    int pickNext(const std::vector<int>& queue, int head) override {
        int best = queue[0];
        int bestDist = std::abs(queue[0] - head);
        for (int r : queue) {
            int d = std::abs(r - head);
            if (d < bestDist) {
                bestDist = d;
                best = r;
            }
        }
        return best;
    }
};

#endif // MINIOS_SSTF_DISK_SCHEDULER_H
