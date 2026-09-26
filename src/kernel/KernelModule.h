#ifndef MINIOS_KERNEL_MODULE_H
#define MINIOS_KERNEL_MODULE_H

// Abstract interface for every pluggable kernel subsystem.
class KernelModule {
public:
    virtual ~KernelModule() = default;
    virtual void initialize() = 0;
    virtual void shutdown() = 0;
};

#endif // MINIOS_KERNEL_MODULE_H
