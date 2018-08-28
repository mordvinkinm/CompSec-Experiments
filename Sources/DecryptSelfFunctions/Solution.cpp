#include <stdio.h>
#include <stdlib.h>

#include "Injector.h"
#include "Io.h"
#include "FunctionEncryptor.h"

const std::string Key = "THIS IS SECRED KEY!";

void method_01(int cnt){
    printf("Hello, I am method 01. I receive integer as parameter, but absolutely useless :(\n");
}

int method_02(std::string data) {
    printf("Hello, I am method_02. I simply calculates string lenth\n");
    int result = data.size();

    return result;
}

int main(int argc, char* argv[]) {
    if (argc < 1) {
        printf("Cannot get app path, something went wrong");
        return EXIT_FAILURE;
    }

    decrypt(&method_01, Key);
    decrypt(&method_02, Key);
    method_01(method_02("abcdefgh"));
    encrypt(&method_01, Key);
    encrypt(&method_02, Key);

    system("pause");
    return EXIT_SUCCESS;
}