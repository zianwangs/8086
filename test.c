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
    
    while (pages--) {
         uint cur = kpgdir;   
        for (int i = 4; i > 0; --i) {
            int shift = (i - 1) * 9 + 12;
            int idx = shift >= 32 ? 0 : ((unsigned int)va_start >> shift) & 0x1FF;
            uint ptr = cur + idx * 2;
            if (i == 1 && pa_start != 0) {
                *ptr = (unsigned int)pa_start | 3;
                pa_start += 1000;
            } else if (*ptr == 0) {
                *ptr = (unsigned int)kalloc() | 3;
            }
            cur = (*ptr) & (~4095U);
        }
        va_start += 1000;
    }
}

