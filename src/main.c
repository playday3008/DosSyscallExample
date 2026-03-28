#include <stdio.h>
#include <conio.h>
#include "dossyscall.h"

int main(void)
{
    int major, minor, hour, min, sec;

    printf("Hello from DOS!\n");

    dos_get_version(&major, &minor);
    printf("DOS version: %d.%d\n", major, minor);

    dos_get_time(&hour, &min, &sec);
    printf("System time: %02d:%02d:%02d\n", hour, min, sec);

    printf("\nPress any key to exit...");
    fflush(stdout);
    getch();

    return 0;
}
