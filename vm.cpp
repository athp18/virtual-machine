#include "vm.h"
#include <stdexcept>
#include <iostream>
#include <cstring>

VirtualMachine::VirtualMachine(size_t stack_size, size_t heap_size, size_t static_size)
    : stack(stack_size), sp(0), fp(0), heap(heap_size), hp(0), static_memory(static_size), pc(0) {}

void VirtualMachine::push(int32_t value) {
    if (sp >= stack.size()) throw std::overflow_error("Stack overflow");
    stack[sp++] = value;
}

int32_t VirtualMachine::pop() {
    if (sp == 0) throw std::underflow_error("Stack underflow");
    return stack[--sp];
}

int32_t VirtualMachine::peek() const {
    if (sp == 0) throw std::underflow_error("Stack is empty");
    return stack[sp - 1];
}

size_t VirtualMachine::allocate(size_t size) {
    if (hp + size > heap.size()) throw std::bad_alloc();
    size_t addr = hp;
    hp += size;
    return addr;
}

void VirtualMachine::free(size_t addr, size_t size) {
    if (addr + size == hp) hp = addr;
}

std::vector<uint8_t> VirtualMachine::read_heap(size_t addr, size_t size) const {
    if (addr + size > heap.size()) throw std::out_of_range("Heap access out of bounds");
    return std::vector<uint8_t>(heap.begin() + addr, heap.begin() + addr + size);
}

void VirtualMachine::write_heap(size_t addr, const std::vector<uint8_t>& data) {
    if (addr + data.size() > heap.size()) throw std::out_of_range("Heap write out of bounds");
    std::copy(data.begin(), data.end(), heap.begin() + addr);
}

void VirtualMachine::call(size_t addr) {
    push(static_cast<int32_t>(pc));
    push(static_cast<int32_t>(fp));
    fp = sp;
    pc = addr;
}

void VirtualMachine::ret() {
    int32_t ret_value = pop();
    sp = fp;
    fp = static_cast<size_t>(pop());
    pc = static_cast<size_t>(pop());
    push(ret_value);
}

bool VirtualMachine::execute_opcode(const std::string& opcode, const std::vector<int32_t>& program) {
    if (opcode == "PUSH") {
        push(program[pc++]);
    } else if (opcode == "POP") {
        pop();
    } else if (opcode == "DUP") {
        push(peek());
    } else if (opcode == "SWAP") {
        int32_t a = pop(), b = pop();
        push(a);
        push(b);
    } else if (opcode == "ADD") {
        int32_t b = pop(), a = pop();
        push(a + b);
    } else if (opcode == "SUB") {
        int32_t b = pop(), a = pop();
        push(a - b);
    } else if (opcode == "MUL") {
        int32_t b = pop(), a = pop();
        push(a * b);
    } else if (opcode == "DIV") {
        int32_t b = pop(), a = pop();
        if (b == 0) throw std::runtime_error("Division by zero");
        push(a / b);
    } else if (opcode == "MOD") {
        int32_t b = pop(), a = pop();
        if (b == 0) throw std::runtime_error("Modulo by zero");
        push(a % b);
    } else if (opcode == "STORE") {
        size_t addr = static_cast<size_t>(pop());
        int32_t value = pop();
        if (addr >= static_memory.size()) throw std::out_of_range("Static memory access out of bounds");
        static_memory[addr] = value;
    } else if (opcode == "LOAD") {
        size_t addr = static_cast<size_t>(pop());
        if (addr >= static_memory.size()) throw std::out_of_range("Static memory access out of bounds");
        push(static_memory[addr]);
    } else if (opcode == "JMP") {
        pc = static_cast<size_t>(program[pc]);
    } else if (opcode == "JZ") {
        size_t addr = static_cast<size_t>(program[pc++]);
        if (pop() == 0) pc = addr;
    } else if (opcode == "JNZ") {
        size_t addr = static_cast<size_t>(program[pc++]);
        if (pop() != 0) pc = addr;
    } else if (opcode == "CALL") {
        call(static_cast<size_t>(program[pc++]));
    } else if (opcode == "RET") {
        ret();
    } else if (opcode == "ALLOC") {
        size_t size = static_cast<size_t>(pop());
        push(static_cast<int32_t>(allocate(size)));
    } else if (opcode == "FREE") {
        size_t size = static_cast<size_t>(pop());
        size_t addr = static_cast<size_t>(pop());
        free(addr, size);
    } else if (opcode == "STORE_HEAP") {
        size_t addr = static_cast<size_t>(pop());
        int32_t value = pop();
        std::vector<uint8_t> data(reinterpret_cast<uint8_t*>(&value), reinterpret_cast<uint8_t*>(&value) + sizeof(value));
        write_heap(addr, data);
    } else if (opcode == "LOAD_HEAP") {
        size_t addr = static_cast<size_t>(pop());
        std::vector<uint8_t> data = read_heap(addr, sizeof(int32_t));
        int32_t value;
        std::memcpy(&value, data.data(), sizeof(value));
        push(value);
    } else if (opcode == "PRINT") {
        std::cout << pop() << std::endl;
    } else if (opcode == "HALT") {
        return false;
    } else {
        throw std::runtime_error("Unknown opcode: " + opcode);
    }
    return true;
}

void VirtualMachine::run(const std::vector<std::string>& opcodes, const std::vector<int32_t>& program) {
    pc = 0;
    while (pc < opcodes.size()) {
        if (!execute_opcode(opcodes[pc++], program)) break;
    }
}
