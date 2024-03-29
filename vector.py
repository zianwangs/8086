NTRAPS = 256
TRAPNO_SET_WITH_ERROR_CODE = {8, 10, 11, 12, 13, 14, 17, 21, 29, 30}

print("; generated by vector.py - do not edit")
print("BITS 64")
print("extern trampoline")
print("global vector")

for i in range(NTRAPS):
    print(f"vector{i}:")
    if i not in TRAPNO_SET_WITH_ERROR_CODE:
        print(f"    push qword 0")
    print(f"    push qword {i}")
    print(f"    jmp trampoline")

print("SECTION .data")
print("vector:")
for i in range(NTRAPS):
    print(f"    dq vector{i}")