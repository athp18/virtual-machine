#include "vm.h"
#include <iostream>
#include <vector>
#include <string>
#include <cstdint>

int main() {
    // Create a virtual machine with 1024 bytes each for stack, heap, and static memory
    VirtualMachine vm(1024, 1024, 1024);

    // Define opcodes for our program
    std::vector<std::string> opcodes = {
        // Main program
        "PUSH", "ALLOC",    // Allocate memory for the array
        "PUSH", "PUSH", "STORE_HEAP",  // Store 5 at index 0 (array length)
        "PUSH", "PUSH", "PUSH", "STORE_HEAP",  // Store 10 at index 1
        "PUSH", "PUSH", "PUSH", "STORE_HEAP",  // Store 3 at index 2
        "PUSH", "PUSH", "PUSH", "STORE_HEAP",  // Store 7 at index 3
        "PUSH", "PUSH", "PUSH", "STORE_HEAP",  // Store 1 at index 4
        "PUSH", "PUSH", "PUSH", "STORE_HEAP",  // Store 9 at index 5
        "DUP", "PUSH", "CALL",  // Call find_max function
        "PRINT",  // Print the result
        "PUSH", "FREE",  // Free the allocated memory
        "HALT",

        // find_max function
        "PUSH", "LOAD_HEAP",  // Load array length
        "PUSH", "STORE",  // Store length in static memory at address 0
        "PUSH", "PUSH", "LOAD_HEAP",  // Load first element as initial max
        "PUSH", "STORE",  // Store initial max in static memory at address 1
        "PUSH", "PUSH", "STORE",  // Initialize loop counter to 1 at address 2
        // Loop start
        "PUSH", "LOAD",  // Load loop counter
        "PUSH", "LOAD",  // Load array length
        "SUB", "JZ",  // If counter == length, exit loop
        "PUSH",  // Jump to after the loop if equal
        "PUSH", "LOAD",  // Load loop counter
        "PUSH", "ADD",  // Add 1 to get current array index
        "PUSH", "ADD",  // Add to base array address
        "LOAD_HEAP",  // Load current array element
        "PUSH", "LOAD",  // Load current max
        "SUB", "JLE",  // If current <= max, skip update
        "PUSH",
        "DUP", "PUSH", "LOAD", "ADD", "PUSH", "STORE",  // Update max
        "PUSH", "PUSH", "LOAD", "PUSH", "ADD", "PUSH", "STORE",  // Increment counter
        "JMP",  // Jump to start of loop
        "PUSH",
        "PUSH", "LOAD",  // Load max value
        "RET"  // Return max value
    };

    // Define the program data
    std::vector<int32_t> program = {
        24,  // Size of array (in bytes)
        0,   // Array base address (to be filled by ALLOC)
        0, 5,  // Array length
        1, 10, // Array elements
        2, 3,
        3, 7,
        4, 1,
        5, 9,
        38,  // Address of find_max function
        0, 0, // Addresses for static memory usage
        1,    // Loop counter initial value
        70,   // Jump target for loop exit
        56,   // Jump target for max value update skip
        38    // Jump target for loop start
    };

    try {
        // Run the program
        std::cout << "Executing program to find maximum value in array..." << std::endl;
        vm.run(opcodes, program);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
