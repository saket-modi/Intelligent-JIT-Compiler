/*
    To compile, run:
    clang++ -S -O1 -emit-llvm input.cpp -o input.ll
    Then run the transpiler to convert the generated IR to machine specific instructions (defined in VM)
    The code doesn't include any libraries to avoid bloat while ll generation
*/
int main() {
    int sum = 0;
    for (int i = 0; i < 10000; i++) 
        sum += i;
    return sum;
}