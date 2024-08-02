#include <stdint.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    char test_string[12] = "Test string";
    size_t test_string_length = strlen(test_string);

    int id = get_uid();

    write(STDOUT_FILENO, "Hello World", 12);

    // while (true)
    //     ;
    return id;
}
