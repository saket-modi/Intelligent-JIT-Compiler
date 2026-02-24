#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <string>

/*
    Data Model Definitions (Instruction set)
    Synchronized with LLVM Transpiler
*/
enum Opcode {
    PUSH = 0x01,
    LOAD = 0x02,
    STORE = 0x03,   // store into virtual registers
    IADD = 0x04,    // pop b, a -> ADD(a, b)
    ILT = 0x05,     // integer less than
    JUMP_IF = 0x06, // conditional jump
    PRINT = 0x07,   // output the top of the stack
    JUMP = 0x08,    // unconditional jump
    HALT = 0xFF     // end execution
};

struct Instruction {
    Opcode op;
    int operand;
};

/*
    Runtime Profiler Module
*/
class Profiler {
public:
    std::unordered_map<int, int> hit_counts;
    std::vector<std::pair<int, Opcode>> execution_trace;

    void record(int pc, Opcode op) {
        hit_counts[pc]++;
        
        // Logging for AI training data
        if (execution_trace.size() < 5000) { // Increased limit for larger loops
            execution_trace.push_back({pc, op});
        }
    }

    std::string opcode_to_txt(Opcode op) {
        switch(op) {
            case PUSH:    return "PUSH";
            case LOAD:    return "LOAD";
            case STORE:   return "STORE";
            case IADD:    return "IADD";
            case ILT:     return "ILT";
            case JUMP_IF: return "JUMP_IF";
            case PRINT:   return "PRINT";
            case JUMP:    return "JUMP";
            case HALT:    return "HALT";
            default:      return "UNKNOWN";
        }
    }

    // Week 7 Bridge: Export data for Python/AI
    void export_trace(const std::string& filename) {
        std::ofstream file(filename);
        file << "pc,opcode\n";
        for (auto const& entry : execution_trace) {
            file << entry.first << "," << (int)entry.second << "\n";
        }
        file.close();
        std::cout << "Trace exported to " << filename << " for AI training.\n";
    }

    void dump_stats(const std::vector<Instruction> &program) {
        std::cout << "\n--- Profiler Report ---\n";
        std::cout << "Hot Spots (PC : Frequency):\n";
        for (auto const& [pc, count] : hit_counts) {
            if (count > 10) { // Threshold to ignore setup code
                std::cout << "  PC[" << pc << "] (" << opcode_to_txt(program[pc].op) << "): " << count << " hits\n";
            }
        }
    }
};

/*
    Interpreter Core
*/
void run_vm(const std::vector<Instruction>& program) {
    std::vector<int> stack; 
    std::vector<int> registers(256, 0); 
    int pc = 0; 
    
    Profiler profiler;

    std::cout << "VM Executing Bytecode...\n";

    while (pc < (int)program.size()) {
        const auto& instr = program[pc];

        // std::cout << "Executing PC: " << pc << " | Op: " << profiler.opcode_to_txt(instr.op) << " | Val: " << instr.operand << std::endl;

        profiler.record(pc, instr.op);

        switch (instr.op) {
            case PUSH: 
                stack.push_back(instr.operand); 
                pc++; 
                break;
            case LOAD: 
                stack.push_back(registers[instr.operand]); 
                pc++; 
                break;
            case STORE: 
                if(!stack.empty()){
                    registers[instr.operand] = stack.back(); 
                    stack.pop_back(); 
                }
                pc++; 
                break;
            case IADD: {
                int b = stack.back(); stack.pop_back();
                int a = stack.back(); stack.pop_back();
                stack.push_back(a + b);
                pc++;
                break;
            }
            case ILT: {
                int b = stack.back(); stack.pop_back();
                int a = stack.back(); stack.pop_back();
                stack.push_back(a < b ? 1 : 0);
                pc++;
                break;
            }
            case JUMP_IF: {
                int condition = stack.back(); stack.pop_back();
                if (condition) pc = instr.operand; 
                else pc++;
                break;
            }
            case JUMP:
                pc = instr.operand;
                break;
            case PRINT:
                std::cout << ">> " << stack.back() << "\n";
                stack.pop_back();
                pc++;
                break;
            case HALT:
                profiler.dump_stats(program);
                profiler.export_trace("execution_trace.csv");
                return;
        }
    }
}

int main() {
    std::vector<Instruction> program;
    std::ifstream inFile("output.txt");
    int opCode, operand;

    while (inFile >> opCode >> operand) {
        program.push_back({static_cast<Opcode>(opCode), operand});
    }
    
    if (program.empty()) {
        std::cout << "Error: No bytecode found in output.txt" << std::endl;
        return -1;
    }

    run_vm(program);
    return 0;
}