#include <stdio.h>

void PrintHDDMessage(int diskNumber, const char* driveLetters, const char* model)
{
    printf("Disk %d (%s, %s) is HDD.\n", diskNumber, driveLetters, model);
}
