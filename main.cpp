#include "Listener.h"
#include <cstdio>

int main(int argv, char* argc[]) {
    if(argv != 2) {
        printf("Right Usage is : %s config\n", argc[0]);
        fflush(stdout);
        return -1;
    }

    Listener listener(argc[1]);
    int res = listener.ReadConfig();
    if(res != 1)
        return -1;
    listener.init();
    listener.start();

    return 0;

}