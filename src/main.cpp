#include <iostream>
#include <limits>
#include "kernel/Kernel.h"

static void printMenu() {
    std::cout << "\n========================================\n";
    std::cout << "           MiniOS Simulator\n";
    std::cout << "========================================\n\n";
    std::cout << "1. Process Management\n";
    std::cout << "2. CPU Scheduling\n";
    std::cout << "3. Memory Management\n";
    std::cout << "4. Paging & Page Replacement\n";
    std::cout << "5. TLB Simulation\n";
    std::cout << "6. Disk Scheduling\n";
    std::cout << "7. Deadlock Management\n";
    std::cout << "8. Synchronization\n";
    std::cout << "9. Run Complete Demo\n";
    std::cout << "0. Exit\n\n";
}

static int readChoice() {
    int choice;
    while (true) {
        std::cout << "Enter choice: ";
        if (std::cin >> choice) return choice;
        if (std::cin.eof()) return 0;
        std::cout << "Invalid input — please enter a number.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

int main() {
    Kernel minios;
    minios.boot();

    int choice;
    do {
        printMenu();
        choice = readChoice();

        switch (choice) {
            case 1: minios.processManagementMenu(); break;
            case 2: minios.cpuSchedulingMenu(); break;
            case 3: minios.memoryManagementMenu(); break;
            case 4: minios.pagingMenu(); break;
            case 5: minios.tlbMenu(); break;
            case 6: minios.diskSchedulingMenu(); break;
            case 7: minios.deadlockMenu(); break;
            case 8: minios.synchronizationMenu(); break;
            case 9: minios.run(); break;
            case 0: std::cout << "\nExiting...\n"; break;
            default: std::cout << "Invalid choice — try again.\n"; break;
        }
    } while (choice != 0);

    minios.exportReport();
    minios.shutdown();

    std::cout << "\nSystem halted.\n";
    return 0;
}
