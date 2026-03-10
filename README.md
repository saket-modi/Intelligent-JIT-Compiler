# Intelligent JIT Compiler

Traditional JIT compilers are reactive and they wait for a counter to exceed an arbitrary threshold (e.g., `count > 1000`) before triggering an optimization pass. **This project is different.**

We utilize a **Predictive JIT** architecture. By employing a Random Forest classifier, our JIT analyzes opcode sequences to predict "hot spots" *before* they become computationally expensive.

---

## Workflow
Our compilation pipeline bridges the gap between C++ performance and Python’s machine learning capabilities:

1.  **Assembler Pipeline:** C++ code is transpiled to LLVM IR, then mapped to our custom 64-bit ISA.
2.  **Profiling:** The Virtual Machine executes the code and logs execution traces to `master_trace.csv`.
3.  **Intelligence Layer:** A Python-based *Random Forest* model analyzes the sequences of opcodes to learn structural "hotness" patterns.
4.  **Inference Bridge:** A lightweight socket-based server provides real-time "hot-spot" triggers to the VM.

---

## 🛠 Prerequisites

Ensure you have the following installed to run the pipeline:
* **C++20 Compiler** (g++, clang++)
* **Python 3.10+**

### Environment Setup
To install the required Python dependencies, run the following command from the project root:

```bash
pip install -r requirements.txt
```

---

## Getting Started

### 1. Data Collection

Generate the master dataset by running the micro-benchmark suite. This script compiles your test programs, runs them, and aggregates the traces into `master_trace.csv`.

```bash
# Ensure you are using a Bash-compatible terminal (e.g., Git Bash)
chmod +x collect_data.sh
./collect_data.sh
```

### 2. Train the Brain

Train the Random Forest model on the aggregated patterns. The model is **address-agnostic**; it ignores specific Program Counters (PCs) and focuses on the sequence of opcodes, allowing it to generalize across different programs.

```bash
python model.py
```

*This will generate `jit_predictor_v1.pkl`.*

### 3. Deploy the Inference Bridge

Start the socket server that will listen for trigger requests from the VM.

```bash
python inference_bridge.py
```

### 4. Execute the VM

In a separate terminal, run your VM. It will now communicate with the bridge in real-time to trigger JIT compilation for predicted hot spots.

```bash
./virtual_machine my_program.txt
```