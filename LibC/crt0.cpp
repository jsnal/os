extern "C" {

int main(int, char**, char**);

int _start(int argc, char* argv[], char* env[]) {
    int ret = main(argc, argv, env);
    return -1;
}

}
