#include <windows.h>
#include <stdio.h>
#include "detect.h"

// forward decl from hdd.c / ssd.c
void PrintHDDMessage(int diskNumber, const char* driveLetters, const char* model);
void PrintSSDMessage(int diskNumber, const char* driveLetters, const char* model);

int main(void)
{
    printf("\n");

    for (int i = 0; i < 10; i++)
    {
        char model[128];
        char letters[128];

        if (!GetDiskModel(i, model, sizeof(model)))
            break;

        GetDriveLetters(i, letters, sizeof(letters));

        int isSSD = IsSSD(i);

        if (isSSD)
            PrintSSDMessage(i, letters, model);
        else
            PrintHDDMessage(i, letters, model);
    }

    return 0;
}
