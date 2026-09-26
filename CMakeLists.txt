cmake_minimum_required(VERSION 3.10)
project(MiniOS VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(MiniOS
    src/main.cpp
    src/process/PCB.cpp
    src/process/Process.cpp
    src/process/UserProcess.cpp
    src/process/SystemProcess.cpp
    src/kernel/ProcessManager.cpp
    src/kernel/Kernel.cpp
    src/memory/MemoryManager.cpp
    src/disk/Disk.cpp
    src/banker/Banker.cpp
    src/tlb/TLB.cpp
    src/sync/Semaphore.cpp
)

target_include_directories(MiniOS PRIVATE src)
