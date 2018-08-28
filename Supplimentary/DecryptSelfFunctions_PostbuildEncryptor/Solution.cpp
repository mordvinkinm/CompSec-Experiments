#include "FunctionEncryptor.h"
#include "Io.h"

// list of function offsets
size_t offsetCnt = 2;
int offsets[2] = { 
    0x610   // method_01
    ,0x630  // method_02
};

const std::string Key = "THIS IS SECRED KEY!";

int main(int argc, char* argv[]) {
    std::string filename;
    std::cout<<"Enter path to file"<<std::endl;
    getline(std::cin, filename);

    byte_array file = read_file(filename);

    for (size_t i = 0; i < offsetCnt; i++){
        char* pointer = file.pointer + offsets[i];

        for (size_t i = 0, key_index = 0; i < file.size; i++, key_index = (key_index + 1) % Key.size()){
            if (*(pointer + i) == PreRetVal && *(pointer + i + 1) == RetVal)
                break;

            *(pointer + i) = *(pointer + i) ^ Key[key_index];
        }
    }

    write_file(filename, file);

    printf("Encryption completed\n");

    system("pause");

    return EXIT_SUCCESS;
}