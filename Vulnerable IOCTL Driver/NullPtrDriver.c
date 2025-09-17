#include <ntddk.h>

#define IOCTL_NULL_DEREF CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct _USER_DATA {
    ULONG Flag;
    PVOID Pointer;
    ULONG Value;
} USER_DATA, *PUSER_DATA;

void DriverUnload(PDRIVER_OBJECT DriverObject) {
    UNICODE_STRING symbolicLink = RTL_CONSTANT_STRING(L"\\DosDevices\\NullPtrDrv");
    IoDeleteSymbolicLink(&symbolicLink);
    IoDeleteDevice(DriverObject->DeviceObject);
    DbgPrint("NullPtrDrv unloaded\n");
}

NTSTATUS DispatchIoctl(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    PVOID inputBuffer = Irp->AssociatedIrp.SystemBuffer;
    ULONG inputLength = irpStack->Parameters.DeviceIoControl.InputBufferLength;

    NTSTATUS status = STATUS_SUCCESS;

    switch (irpStack->Parameters.DeviceIoControl.IoControlCode) {
        case IOCTL_NULL_DEREF:
            {
                PUSER_DATA userData = (PUSER_DATA)inputBuffer;
                
                // 취약점: 포인터 검증 없이 접근
                if (userData && inputLength >= sizeof(USER_DATA)) {
                    if (userData->Flag == 0x1337) {
                        // 널 포인터 역참조 가능성
                        *(ULONG*)userData->Pointer = userData->Value;  // 위험!
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
    UNICODE_STRING deviceName = RTL_CONSTANT_STRING(L"\\Device\\NullPtrDrv");
    UNICODE_STRING symbolicLink = RTL_CONSTANT_STRING(L"\\DosDevices\\NullPtrDrv");

    NTSTATUS status = IoCreateDevice(DriverObject, 0, &deviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &deviceObject);
    if (!NT_SUCCESS(status)) return status;

    status = IoCreateSymbolicLink(&symbolicLink, &deviceName);
    if (!NT_SUCCESS(status)) {
        IoDeleteDevice(deviceObject);
        return status;
    }

    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchIoctl;
    DriverObject->DriverUnload = DriverUnload;

    DbgPrint("NullPtrDrv loaded\n");
    return STATUS_SUCCESS;
}
