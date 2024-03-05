#include "user.h"
int main() {
//    void(*prints)(char*) = 0x80002131;
//    prints("hello world");

    echo("Hello world!");
    yield();
    echo("Hello world!");
    yield();
    while (1);
}