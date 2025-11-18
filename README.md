# DiskTypeDetector
This tool enumerates the physical disks connected to Windows and determines whether each disk is an **HDD** or **SSD**.

# Features
- Physical disk enumeration using Windows API
- Self-reporting for accurate SSD/HDD identification
- Aggregating multiple drive letters in Task Manager format
- Directly reading physical drives with administrator privileges
- Pure C code compatible with C89 (small size, fast)

# Build command
A _**Visual Studio Developer Command Prompt**_ is required.
```
cl /EHsc /Fe:build\DiskTypeDetector.exe src\main.c src\detect.c src\ssd.c src\hdd.c
```
