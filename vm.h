#ifndef VIRTUAL_MACHINE_H
#define VIRTUAL_MACHINE_H

#include <vector>
#include <cstdint>
#include <string>

class VirtualMachine {
private:
    std::vector<int32_t> stack;
    size_t sp;
    size_t fp;
    std::vector<uint8_t> heap;
    size_t hp;
    std::vector<int32_t> static_memory;
    size_t pc;

public:
    VirtualMachine(size_t stack_size = 1024, size_t heap_size = 4096, size_t static_size = 256);

    void push(int32_t value);
    int32_t pop();
    int32_t peek() const;
    
    size_t allocate(size_t size);
    void free(size_t addr, size_t size);
    std::vector<uint8_t> read_heap(size_t addr, size_t size) const;
    void write_heap(size_t addr, const std::vector<uint8_t>& data);

    void call(size_t addr);
    void ret();

    bool execute_opcode(const std::string& opcode, const std::vector<int32_t>& program);
    void run(const std::vector<std::string>& opcodes, const std::vector<int32_t>& program);
};

#endif
