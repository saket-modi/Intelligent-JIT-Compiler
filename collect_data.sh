#!/bin/bash
# Stop the script immediately if any command fails
set -e

echo "[*] Cleaning and building project..."
make clean
make all

echo "[*] Build successful. Starting data collection..."

# Configuration
TEST_FILES=("simple_loop.cpp" "step_loop.cpp" "nested_loop.cpp")
MASTER_FILE="master_trace.csv"

# 1. Initialize Master File with a clean header
echo "pc,opcode" > $MASTER_FILE

echo "[*] Building Master Dataset..."

for SRC in "${TEST_FILES[@]}"
do
    echo "    -> Processing $SRC"
    
    # Step A: Generate LLVM IR (using clang++)
    clang++ -S -emit-llvm "$SRC" -o input.ll
    
    # Step B: Transpile to Custom Bytecode
    # (Assuming your transpiler reads input.ll and writes output.txt)
    ./transpiler > /dev/null
    
    # Step C: Execute in VM to generate trace
    # (Assuming your VM reads output.txt and writes execution_trace.csv)
    ./virtual_machine > /dev/null
    
    # Step D: Append to Master (Skip the header of individual traces)
    if [ -f "execution_trace.csv" ]; then
        tail -n +2 execution_trace.csv >> $MASTER_FILE
    else
        echo "    [!] Error: execution_trace.csv missing for $SRC"
    fi
done

echo "[*] Done. Total samples in $MASTER_FILE: $(wc -l < $MASTER_FILE)"