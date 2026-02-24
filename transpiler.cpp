#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <algorithm>

using namespace std;

enum Opcode { PUSH = 0x01, LOAD = 0x02, STORE = 0x03, IADD = 0x04, ILT = 0x05, JUMP_IF = 0x06, PRINT = 0x07, JUMP = 0x08, HALT = 0xFF };

struct Instruction { Opcode op; int operand; };

unordered_map<string, int> label_map;

string clean(string s) {
    s.erase(remove(s.begin(), s.end(), ','), s.end());
    s.erase(remove(s.begin(), s.end(), '%'), s.end());
    if (!s.empty() && s.back() == ':') s.pop_back();
    return s;
}

// Helper to decide if an operand is a Register (%1) or Literal (1000)
void emit_load_or_push(string operand, vector<Instruction>& program) {
    bool is_reg = (operand.find('%') != string::npos);
    string val = clean(operand);
    if (is_reg) {
        program.push_back({LOAD, stoi(val)});
    } else {
        program.push_back({PUSH, stoi(val)});
    }
}

int main() {
    vector<Instruction> program;
    ifstream file("./input.ll");
    if (!file.is_open()) return -1;

    string line;
    vector<string> lines;
    
    // --- Pass 1: Capture Labels ---
    int current_pc = 0;
    while (getline(file, line)) {
        lines.push_back(line);
        size_t first = line.find_first_not_of(" \t");
        if (string::npos == first) continue;
        string trimmed = line.substr(first);
        
        stringstream ss(trimmed);
        string word; ss >> word;

        if (word.find(':') != string::npos && word.find("define") == string::npos) {
            label_map[clean(word)] = current_pc;
            continue; 
        }

        // Exact instruction count matching Pass 2
        if (trimmed.find("store ") != string::npos) current_pc += 2;
        else if (trimmed.find(" load ") != string::npos) current_pc += 2;
        else if (trimmed.find(" icmp ") != string::npos) current_pc += 4;
        else if (trimmed.find(" add ") != string::npos) current_pc += 4;
        else if (trimmed.find("br i1 ") != string::npos) current_pc += 3;
        else if (trimmed.find("br label ") != string::npos) current_pc += 1;
        else if (trimmed.find("ret ") != string::npos) current_pc += 3;
    }

    // --- Pass 2: Generate Bytecode ---
    for (string& raw_line : lines) {
        size_t first = raw_line.find_first_not_of(" \t");
        if (string::npos == first) continue;
        string line = raw_line.substr(first);
        
        if (line.find(":") != string::npos && line.find("define") == string::npos) continue;

        // Extract the destination register (e.g. "%5") if it's an assignment
        string dest = "";
        size_t eq_pos = line.find("=");
        if (eq_pos != string::npos) {
            string lhs = line.substr(0, eq_pos);
            lhs.erase(remove_if(lhs.begin(), lhs.end(), ::isspace), lhs.end());
            dest = clean(lhs);
        }

        if (line.find("store ") != string::npos) {
            stringstream ss(line); string t, val_str, ptr_str;
            while(ss >> t) { if (t == "i32") ss >> val_str; if (t == "ptr") ss >> ptr_str; }
            emit_load_or_push(val_str, program);
            program.push_back({STORE, stoi(clean(ptr_str))});
        }
        else if (line.find(" load ") != string::npos) {
            stringstream ss(line); string t, ptr_str;
            while(ss >> t) { if (t == "ptr") ss >> ptr_str; }
            program.push_back({LOAD, stoi(clean(ptr_str))});
            program.push_back({STORE, stoi(dest)}); // Save to virtual register!
        }
        else if (line.find(" icmp ") != string::npos) {
            stringstream ss(line); string t, op1, op2;
            while(ss >> t) { if (t == "i32") { ss >> op1 >> op2; break; } }
            emit_load_or_push(op1, program);
            emit_load_or_push(op2, program);
            program.push_back({ILT, 0});
            program.push_back({STORE, stoi(dest)}); // Save comparison result!
        }
        else if (line.find(" add ") != string::npos) {
            stringstream ss(line); string t, op1, op2;
            while(ss >> t) { if (t == "i32") { ss >> op1 >> op2; break; } }
            emit_load_or_push(op1, program);
            emit_load_or_push(op2, program);
            program.push_back({IADD, 0});
            program.push_back({STORE, stoi(dest)}); // Save math result!
        }
        else if (line.find("br i1 ") != string::npos) {
            stringstream ss(line); string t, cond, l_true, l_false;
            while(ss >> t) {
                if (t == "i1") { ss >> cond; ss >> t >> l_true; ss >> t >> l_false; break; }
            }
            emit_load_or_push(cond, program);
            program.push_back({JUMP_IF, label_map[clean(l_true)]});
            program.push_back({JUMP, label_map[clean(l_false)]});
        }
        else if (line.find("br label ") != string::npos) {
            stringstream ss(line); string t, l_name;
            while(ss >> t) { if (t == "label") { ss >> l_name; break; } }
            program.push_back({JUMP, label_map[clean(l_name)]});
        }
        else if (line.find("ret ") != string::npos) {
            stringstream ss(line); string t, val_str;
            while(ss >> t) { if (t == "i32") { ss >> val_str; break; } }
            emit_load_or_push(val_str, program);
            program.push_back({PRINT, 0});
            program.push_back({HALT, 0});
        }
    }

    ofstream outFile("output.txt");
    for (const auto& instr : program) outFile << (int)instr.op << " " << instr.operand << "\n";
    outFile.close();
    cout << "Transpilation complete. Check output.txt." << endl;

    return 0;
}