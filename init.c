
int main() {
//    void(*prints)(char*) = 0x80002131;
//    prints("hello world");
    asm volatile("int $0x80");
    asm volatile("int $0x80");
    while (1);
}