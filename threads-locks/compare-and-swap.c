#include <stdio.h>

int global = 0;

char compare_and_swap(int *ptr, int old, int new) {
    unsigned char ret;
    // Note that sete sets a ’byte’ not the word
    __asm__ __volatile__ (
    /*
    https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html
    1. here only use outside passed-in parameters as "InputOperands"
    2. ignore this: https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html#x86-Operand-Modifiers
        here `QImode` is enough for `unsigned char`.
        - Above link is about "h" in '"xchg %h0, %b0" : "+a" (num)'
    3. https://github.com/gcc-mirror/gcc/blob/master/gcc/config/i386/constraints.md from https://stackoverflow.com/a/34460377/21294350
    4. "m" -> memory by https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html#Output-Operands
    5. `"a" (old)` is due to `cmpxchg` compare with "a" register like rax.
    */
	" lock\n\t"
	" cmpxchgl %2,%1\n"
	" sete %0\n"
	: "=q" (ret), "=m" (*ptr)
	: "r" (new), "m" (*ptr), "a" (old)
	: "memory");
    return ret;
}

int main(int argc, char *argv[]) {
    printf("before successful cas: %d\n", global);
    int success = compare_and_swap(&global, 0, 100);
    printf("after successful cas: %d (success: %d)\n", global, success);
    
    printf("before failing cas: %d\n", global);
    success = compare_and_swap(&global, 0, 200);
    printf("after failing cas: %d (old: %d)\n", global, success);

    return 0;
}

