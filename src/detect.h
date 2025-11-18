#ifndef DETECT_H
#define DETECT_H

#include <windows.h>

int GetDiskModel(int diskNumber, char* outModel, DWORD outSize);
int GetDriveLetters(int diskNumber, char* outLetters, DWORD outSize);
int IsSSD(int diskNumber);

#endif