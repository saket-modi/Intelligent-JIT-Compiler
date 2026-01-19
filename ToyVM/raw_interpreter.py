import time

# Define Bytecode Instructions
# IADD: Add two numbers, PRINT: Output top of stack, LOOP: Jump back
# ------------------------------------------------------------------
PROGRAM = [
    ("PUSH", 0), ("STORE", 0),      # result = 0
    ("PUSH", 1), ("STORE", 1),      # i = 1
    ("PUSH", 1000000), ("STORE", 2), # limit = 1000000
    
    # --- LOOP START (Index 6) ---
    ("LOAD", 0), ("LOAD", 1), ("IADD", None), ("STORE", 0), # result += i
    ("LOAD", 1), ("PUSH", 1), ("IADD", None), ("STORE", 1), # i += 1
    ("LOAD", 1), ("LOAD", 2), ("ILT", None),                # i < limit?
    ("JUMP_IF", 6),                                         # If yes, jump to index 6
    
    ("LOAD", 0), ("PRINT", None), ("HALT", None)
]

class ToyVM:
    def __init__(self, code):
        self.code = code
        self.stack = []
        self.variables = {}  # Mock registers/memory
        self.pc = 0          # Program Counter
        self.profile_counts = {} # Runtime Profiler

    def run(self):
        start_time = time.time()
        while self.pc < len(self.code):
            op, val = self.code[self.pc]
            
            # Track how many times each instruction index (referenced by its PC) is hit 
            self.profile_counts[self.pc] = self.profile_counts.get(self.pc, 0) + 1
            
            if op == "PUSH":
                self.stack.append(val)
            elif op == "IADD":
                b = self.stack.pop()
                a = self.stack.pop()
                self.stack.append(a + b)
            elif op == "LOAD":
                # load the value at the memory location at the top of the stack
                # the value will be 0 if not assigned earlier
                self.stack.append(self.variables.get(val, 0))
            elif op == "STORE":
                self.variables[val] = self.stack.pop()
            elif op == "ILT":
                b = self.stack.pop()
                a = self.stack.pop()
                self.stack.append(a < b)
            elif op == "JUMP_IF": # if top of the stack has 'True' from ops like ILT
                target = val
                if self.stack.pop():
                    self.pc = target
                    continue
            elif op == "PRINT":
                print(f"Output: {self.stack.pop()}")
            elif op == "HALT":
                break
            
            self.pc += 1
        
        end_time = time.time()
        print(f"Interpreter Execution Time: {end_time - start_time:.4f} seconds")
        return self.profile_counts

# Execute the VM
vm = ToyVM(PROGRAM)
hot_spots = vm.run()

# Identify the "Hot Spot" (The Loop) 
print("\n--- Profiler Report (Top 3 Hot Instructions) ---")
sorted_spots = sorted(hot_spots.items(), key=lambda x: x[1], reverse=True)
for i in range(3):
    pc, count = sorted_spots[i]
    print(f"Instruction {pc} ({PROGRAM[pc][0]}): Executed {count} times")