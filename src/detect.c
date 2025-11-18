#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "detect.h"

// Trim whitespace in-place
static void trim_inplace(char* s)
{
    if (!s) return;

    char* start = s;
    while (*start && isspace((unsigned char)*start)) start++;

    char* end = start + strlen(start);
    while (end > start && isspace((unsigned char)*(end - 1))) end--;
    *end = '\0';

    if (start != s)
        memmove(s, start, (size_t)(end - start + 1));
}

// Get Disk model name
int GetDiskModel(int diskNumber, char* outModel, DWORD outSize)
{
    char path[64];
    sprintf(path, "\\\\.\\PhysicalDrive%d", diskNumber);

    HANDLE hDisk = CreateFileA(
        path, 0,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING,
        0, NULL
    );

    if (hDisk == INVALID_HANDLE_VALUE)
        return 0;

    STORAGE_PROPERTY_QUERY query = {0};
    query.PropertyId = StorageDeviceProperty;
    query.QueryType = PropertyStandardQuery;

    BYTE buffer[1024];
    DWORD bytes = 0;

    BOOL ok = DeviceIoControl(
        hDisk,
        IOCTL_STORAGE_QUERY_PROPERTY,
        &query, sizeof(query),
        buffer, sizeof(buffer),
        &bytes,
        NULL
    );

    CloseHandle(hDisk);
    if (!ok) return 0;

    STORAGE_DEVICE_DESCRIPTOR* desc = (STORAGE_DEVICE_DESCRIPTOR*)buffer;

    if (desc->ProductIdOffset == 0 || desc->ProductIdOffset >= bytes)
        return 0;

    char tmp[256] = {0};
    snprintf(tmp, sizeof(tmp), "%s", (char*)buffer + desc->ProductIdOffset);

    trim_inplace(tmp);

    if (tmp[0] == '\0')
        snprintf(outModel, outSize, "Unknown");
    else
        snprintf(outModel, outSize, "%s", tmp);

    return 1;
}

// Get Logical Drive Letter
int GetDriveLetters(int diskNumber, char* outLetters, DWORD outSize)
{
    DWORD drives = GetLogicalDrives();
    if (!drives) {
        snprintf(outLetters, outSize, "NoLetter");
        return 0;
    }

    outLetters[0] = '\0';
    int found = 0;

    for (char letter = 'A'; letter <= 'Z'; letter++)
    {
        if (!(drives & (1 << (letter - 'A'))))
            continue;

        char volumePath[16];
        sprintf(volumePath, "\\\\.\\%c:", letter);

        HANDLE hVol = CreateFileA(
            volumePath, 0,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, OPEN_EXISTING,
            0, NULL
        );

        if (hVol == INVALID_HANDLE_VALUE)
            continue;

        STORAGE_DEVICE_NUMBER sdn;
        DWORD bytes = 0;

        BOOL ok = DeviceIoControl(
            hVol,
            IOCTL_STORAGE_GET_DEVICE_NUMBER,
            NULL, 0,
            &sdn, sizeof(sdn),
            &bytes,
            NULL
        );

        CloseHandle(hVol);
        if (!ok) continue;

        if ((int)sdn.DeviceNumber == diskNumber)
        {
            if (found > 0)
                strncat(outLetters, " ", outSize - strlen(outLetters) - 1);

            char tmp[8];
            snprintf(tmp, sizeof(tmp), "%c:", letter);
            strncat(outLetters, tmp, outSize - strlen(outLetters) - 1);

            found++;
        }
    }

    if (!found) {
        snprintf(outLetters, outSize, "NoLetter");
        return 0;
    }

    return found;
}

// Check SSD
int IsSSD(int diskNumber)
{
    char path[64];
    sprintf(path, "\\\\.\\PhysicalDrive%d", diskNumber);

    HANDLE hDisk = CreateFileA(
        path, 0,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING,
        0, NULL
    );

    if (hDisk == INVALID_HANDLE_VALUE)
        return 0;

    STORAGE_PROPERTY_QUERY query = {0};
    query.PropertyId = StorageDeviceSeekPenaltyProperty;
    query.QueryType = PropertyStandardQuery;

    BYTE buffer[256];
    DWORD bytes = 0;

    BOOL ok = DeviceIoControl(
        hDisk,
        IOCTL_STORAGE_QUERY_PROPERTY,
        &query, sizeof(query),
        buffer, sizeof(buffer),
        &bytes,
        NULL
    );

    CloseHandle(hDisk);
    if (!ok) return 0;

    DEVICE_SEEK_PENALTY_DESCRIPTOR* seek = (DEVICE_SEEK_PENALTY_DESCRIPTOR*)buffer;

    return (seek->IncursSeekPenalty == FALSE); // SSD=1, HDD=0
}
