#include "dossyscall.h"
#include <i86.h>

/* Demo 1: DOS syscall via library functions (int86/int386)
   INT 21h / AH=30h — Get DOS Version */
void dos_get_version(int *major, int *minor)
{
    union REGS regs;

    regs.h.ah = 0x30;
#if defined(_M_I86)
    int86(0x21, &regs, &regs);
#else
    int386(0x21, &regs, &regs);
#endif

    *major = regs.h.al;
    *minor = regs.h.ah;
}

/* Demo 2: DOS syscall via inline assembly (#pragma aux)
   INT 21h / AH=2Ch — Get System Time
   Returns: CH=hour, CL=min, DH=sec, DL=1/100s
   Packs result into unsigned long: (hour:min):(sec:hundredths) */
#if defined(_M_I86)
static unsigned long dos_get_time_raw(void);
#pragma aux dos_get_time_raw = \
    "mov ah, 2Ch"              \
    "int 21h"                  \
    "mov ax, dx"               \
    "mov dx, cx"               \
    value [dx ax]              \
    modify [cx];
#else
static unsigned long dos_get_time_raw(void);
#pragma aux dos_get_time_raw = \
    "mov ah, 2Ch"              \
    "int 21h"                  \
    "movzx eax, cx"            \
    "shl eax, 16"              \
    "mov ax, dx"               \
    value [eax]                \
    modify [ecx edx];
#endif

void dos_get_time(int *hour, int *min, int *sec)
{
    unsigned long raw = dos_get_time_raw();
    *hour = (raw >> 24) & 0xFF;
    *min  = (raw >> 16) & 0xFF;
    *sec  = (raw >> 8)  & 0xFF;
}
