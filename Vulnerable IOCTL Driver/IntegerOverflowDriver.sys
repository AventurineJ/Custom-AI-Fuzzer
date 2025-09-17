#include <ntddk.h>

#define IOCTL_INTEGER_OVERFLOW CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct _ALLOC_REQUEST {
    ULONG Size;
    ULONG Count;
} ALLOC_REQUEST, *PALLOC_REQUEST;

void DriverUnload(PDRIVER_OBJECT DriverObject) {
    UNICODE_STRING symbolicLink = RTL_CONSTANT_STRING(L"\\DosDevices\\IntegerOverflowDrv");
    IoDeleteSymbolicLink(&symbolicLink);
    IoDeleteDevice(DriverObject->DeviceObject);
    DbgPrint("IntegerOverflowDrv unloaded\n");
}

NTSTATUS DispatchIoctl(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    PVOID inputBuffer = Irp->AssociatedIrp.SystemBuffer;
    ULONG inputLength = irpStack->Parameters.DeviceIoControl.InputBufferLength;

    NTSTATUS status = STATUS_SUCCESS;

    switch (irpStack->Parameters.DeviceIoControl.IoControlCode) {
        case IOCTL_INTEGER_OVERFLOW:
            {
                PALLOC_REQUEST req = (PALLOC_REQUEST)inputBuffer;
                
                if (req && inputLength >= sizeof(ALLOC_REQUEST)) {
                    // 취약점: 정수 오버플로우
                    ULONG totalSize = req->Size * req->Count;  // 오버플로우 가능성
                    
                    // 잘못된 검증
                    if (totalSize < 0x1000000) {  // 16MB 제한
                        PVOID buffer = ExAllocatePoolWithTag(NonPagedPool, totalSize, 'OVFL');
                        if (buffer) {
                            DbgPrint("Allocated %lu bytes\n", totalSize);
                            ExFreePoolWithTag(buffer, 'OVFL');
                        }
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
    UNICODE_STRING deviceName = RTL_CONSTANT_STRING(L"\\Device\\IntegerOverflowDrv");
    UNICODE_STRING symbolicLink = RTL_CONSTANT_STRING(L"\\DosDevices\\IntegerOverflowDrv");

    NTSTATUS status = IoCreateDevice(DriverObject, 0, &deviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &deviceObject);
    if (!NT_SUCCESS(status)) return status;

    status = IoCreateSymbolicLink(&symbolicLink, &deviceName);
    if (!NT_SUCCESS(status)) {
        IoDeleteDevice(deviceObject);
        return status;
    }

    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchIoctl;
    DriverObject->DriverUnload = DriverUnload;

    DbgPrint("IntegerOverflowDrv loaded\n");
    return STATUS_SUCCESS;
}
