#include "macros.h"
#include "types.h"

struct freelist {
    void* addr;
    struct freelist* next;
};

struct freelist* kmem = NULL;

void kinit(void* start, void* end) {
    for (void* addr = PGROUNDUP((uint64_t)start); addr < PGROUNDDOWN((uint64_t)end); addr += PG_SIZE) {
        struct freelist* cur = addr;
        cur->addr = addr;
        cur->next = kmem;
        kmem = cur;
    }
}


void* kalloc() {
    if (kmem == NULL) {
        return NULL;
    }
    uint64_t* page = kmem->addr;
    kmem = kmem->next;
    for (int i = 0; i < PG_SIZE / sizeof(uint64_t); ++i)
        page[i] = 0;
    return page;
}

void* kfree(void* addr) {
    struct freelist* cur = addr;
    cur->addr = addr;
    cur->next = kmem;
    kmem = cur;
}