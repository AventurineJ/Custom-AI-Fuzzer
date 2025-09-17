#include <ntddk.h>

#define IOCTL_PRIVILEGE_BYPASS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x804, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct _PRIVILEGE_DATA {
    ULONG AdminOnlyFlag;
    ULONG SensitiveData;
} PRIVILEGE_DATA, *PPRIVILEGE_DATA;

void DriverUnload(PDRIVER_OBJECT DriverObject) {
    UNICODE_STRING symbolicLink = RTL_CONSTANT_STRING(L"\\DosDevices\\PrivilegeDrv");
    IoDeleteSymbolicLink(&symbolicLink);
    IoDeleteDevice(DriverObject->DeviceObject);
    DbgPrint("PrivilegeDrv unloaded\n");
}

NTSTATUS DispatchIoctl(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    PVOID inputBuffer = Irp->AssociatedIrp.SystemBuffer;
    ULONG inputLength = irpStack->Parameters.DeviceIoControl.InputBufferLength;

    NTSTATUS status = STATUS_SUCCESS;

    switch (irpStack->Parameters.DeviceIoControl.IoControlCode) {
        case IOCTL_PRIVILEGE_BYPASS:
            {
                PPRIVILEGE_DATA privData = (PPRIVILEGE_DATA)inputBuffer;
                
                if (privData && inputLength >= sizeof(PRIVILEGE_DATA)) {
                    // 취약점: 권한 검사 없음
                    if (privData->AdminOnlyFlag == 0xDEADBEEF) {
                        // 관리자만 접근해야 하는 민감한 작업
                        DbgPrint("Sensitive operation executed: 0x%08X\n", privData->SensitiveData);
                        // 예: 시스템 설정 변경, 메모리 덤프 등
                    }
                }
            }
            break;
        default:
            status = STATUS_INVALID_DEVICE_REQUEST;
            break;
    }

    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return status;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
    PDEVICE_OBJECT deviceObject;
    UNICODE_STRING deviceName = RTL_CONSTANT_STRING(L"\\Device\\PrivilegeDrv");
    UNICODE_STRING symbolicLink = RTL_CONSTANT_STRING(L"\\DosDevices\\PrivilegeDrv");

    NTSTATUS status = IoCreateDevice(DriverObject, 0, &deviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &deviceObject);
    if (!NT_SUCCESS(status)) return status;

    status = IoCreateSymbolicLink(&symbolicLink, &deviceName);
    if (!NT_SUCCESS(status)) {
        IoDeleteDevice(deviceObject);
        return status;
    }

    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchIoctl;
    DriverObject->DriverUnload = DriverUnload;

    DbgPrint("PrivilegeDrv loaded\n");
    return STATUS_SUCCESS;
}
