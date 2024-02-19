typedef unsigned int* uint;

uint kpgdir = 0x100000;
uint alloc = 0x101000;

uint kalloc() {
    uint ans = alloc;
    alloc += 0x400;
    for (int i = 0; i < 0x400; ++i) {
        ans[i] = 0;
    }
    return ans;
}

void mappages(uint va_start, uint pa_start, int pages) {
    uint cur = kpgdir;    
    while (pages--) {
        for (int i = 4; i > 0; --i) {
            int idx = ((unsigned int)va_start >> ((i - 1) * 9 + 12)) & 0x1FF;
            uint ptr = cur + idx * 2;
            if (i == 1 && pa_start != 0) {
                *ptr = (unsigned int)pa_start;
            } else if (*ptr == 0) {
                *ptr = kalloc();
            }
            cur = (uint)*ptr;
        }
    }
}

