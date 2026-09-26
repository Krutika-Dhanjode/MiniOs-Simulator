#ifndef MINIOS_PROCESS_STATE_H
#define MINIOS_PROCESS_STATE_H

enum class ProcessState {
    NEW,
    READY,
    RUNNING,
    WAITING,
    TERMINATED
};

inline const char* toString(ProcessState state) {
    switch (state) {
        case ProcessState::NEW:        return "NEW";
        case ProcessState::READY:      return "READY";
        case ProcessState::RUNNING:    return "RUNNING";
        case ProcessState::WAITING:    return "WAITING";
        case ProcessState::TERMINATED: return "TERMINATED";
    }
    return "UNKNOWN";
}

#endif // MINIOS_PROCESS_STATE_H
