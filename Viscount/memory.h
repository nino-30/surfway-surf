#include <cstdint>
#include <cstdio>
#include <cstring>
uintptr_t GetBaseAdress(const char* libname) {
    uintptr_t base = 0;
    char line[1024];
    FILE* file = fopen("/proc/self/maps", "r");
    if (!file) {
        return 0;
    }
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, libname)) {
            if (sscanf(line, "%lx-%*lx", &base) == 1) {
                fclose(file);
                return base;
            }
        } 
    }
    fclose(file);
    return 0;
}

void patchMemory(uintptr_t address, unsigned char byte[], size_t size) {

    size_t pageSize = sysconf(_SC_PAGESIZE);
    uintptr_t pageStart = address & ~(pageSize - 1);
    if (mprotect((void*)pageStart, pageSize, PROT_READ | PROT_WRITE | PROT_EXEC) != 0) {
        perror("mprotect");
        return;
    }
    memcpy((void*)address, byte, size);
    mprotect((void*)pageStart, pageSize, PROT_READ | PROT_EXEC);
    __builtin_____clear_cache((char*)pageStart, (char*)(pageStart + pageSize));
    
}
