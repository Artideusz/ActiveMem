#include <windows.h>
#include <stdio.h>

int main(void) {
    BYTE* memory = (BYTE*)VirtualAlloc(NULL, 512, MEM_COMMIT, PAGE_READWRITE);
    memory[0] = 'H';
    memory[1] = 'e';
    memory[2] = 'l';
    memory[3] = 'l';
    memory[4] = 'o';
    memory[5] = '\0';
    printf("%s\n", memory);
    VirtualFree(memory, 512, MEM_RELEASE);
    return 0;
};