#include <stdio.h>

void PrintSSDMessage(int diskNumber, const char* driveLetters, const char* model)
{
    printf("Disk %d (%s, %s) is SSD.\n", diskNumber, driveLetters, model);
}
