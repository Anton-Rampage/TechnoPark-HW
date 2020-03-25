#include <iostream>
#include <unistd.h>

int main() {
    char buffer[256];
    while (!feof(stdin)) {
        size_t size = fread(buffer, 1, sizeof(buffer), stdin);
        fwrite(buffer, 1, size, stdout);
    }
    return 0;
}

