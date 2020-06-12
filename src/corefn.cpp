#include <cstring>
#include <iostream>

extern "C" {
    void print(char *c) {
        std::cout << c;
    }

    void printi(std::uint64_t i) {
        std::cout << i;
    }

    std::uint8_t *allocaRecord(std::uint64_t size) { 
        return (std::uint8_t *)malloc(size);
    }

    std::uint8_t *allocaArray(std::uint64_t size, std::uint64_t elementSize) {
        return (std::uint8_t *)malloc(size * elementSize);
    }

    void flush() { std::cout.flush(); }

    int ord(char *c) {
        if (*c > 127 || *c < 0)
            return -1;
        else
            return (int)*c;
    }

    char *chr(int c) {
        if (c > 127 || c < 0) exit(-1);
        return new char[2]{(char)(c), '\0'};
    }

    int size(char *c) { return std::strlen(c); }

    char *substring(char *s, int first, int n) {
        char *result = new char[n + 1];
        memcpy(result, s + first, n);
        result[n] = '\0';
        return result;
    }

    char *concat(char *s1, char *s2) {
        auto len1 = strlen(s1), len2 = strlen(s2);
        auto len = len1 + len2;
        char *result = new char[len + 1];
        memcpy(result, s1, len1);
        memcpy(result + len1, s2, len2);
        result[len] = '\0';
        return result;
    }

}