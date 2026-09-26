#ifndef MINIOS_PAGE_H
#define MINIOS_PAGE_H

// A virtual page: owned by a process, identified by its page number.
class Page {
public:
    Page() = default;
    Page(int pid, int pageNumber) : pid_(pid), pageNumber_(pageNumber) {}

    int getPid() const { return pid_; }
    int getPageNumber() const { return pageNumber_; }

private:
    int pid_ = -1;
    int pageNumber_ = -1;
};

#endif // MINIOS_PAGE_H
