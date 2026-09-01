.globl _start
.text
_start:
    mov w0, #1
    ret
    mov w1, #0x43fa0000
    fmov s0, w1
    ret
