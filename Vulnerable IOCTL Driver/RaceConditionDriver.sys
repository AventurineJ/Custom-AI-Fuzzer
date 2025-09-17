#include <ntddk.h>

#define IOCTL_RACE_CONDITION CTL_CODE(FILE_DEVICE_UNKNOWN, 0x803, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct _SHARED_DATA {
    LONG Counter;
    ULONG Data[100];
} SHARED_DATA, *PSHARED_DATA;

// 전역 공유 데이터
SHARED_DATA g_SharedData = {0};
FAST_MUTEX g_FastMutex;

void DriverUnload(PDRIVER_OBJECT DriverObject) {
    UNICODE_STRING symbolicLink = RTL_CONSTANT_STRING(L"\\DosDevices\\RaceConditionDrv");
    IoDeleteSymbolicLink(&symbolicLink);
    IoDeleteDevice(DriverObject->DeviceObject);
    DbgPrint("RaceConditionDrv unloaded\n");
}

NTSTATUS DispatchIoctl(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    PVOID inputBuffer = Irp->AssociatedIrp.SystemBuffer;
    ULONG inputLength = irpStack->Parameters.DeviceIoControl.InputBufferLength;

    NTSTATUS status = STATUS_SUCCESS;

    switch (irpStack->Parameters.DeviceIoControl.IoControlCode) {
        case IOCTL_RACE_CONDITION:
            {
                ULONG index = 0;
                ULONG value = 0;
                
                if (inputBuffer && inputLength >= sizeof(ULONG) * 2) {
                    index = ((PULONG)inputBuffer)[0];
                    value = ((PULONG)inputBuffer)[1];
                    
                    // 취약점: 뮤텍스 없이 접근 - 경쟁 조건
                    if (index < 100) {
                        g_SharedData.Data[index] = value;  // 경쟁 조건 발생 지점
                        InterlockedIncrement(&g_SharedData.Counter);
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
    UNICODE_STRING deviceName = RTL_CONSTANT_STRING(L"\\Device\\RaceConditionDrv");
    UNICODE_STRING symbolicLink = RTL_CONSTANT_STRING(L"\\DosDevices\\RaceConditionDrv");

    NTSTATUS status = IoCreateDevice(DriverObject, 0, &deviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &deviceObject);
    if (!NT_SUCCESS(status)) return status;

    status = IoCreateSymbolicLink(&symbolicLink, &deviceName);
    if (!NT_SUCCESS(status)) {
        IoDeleteDevice(deviceObject);
        return status;
    }

    // 뮤텍스 초기화 (하지만 사용하지 않음 - 의도적 결함)
    ExInitializeFastMutex(&g_FastMutex);

    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchIoctl;
    DriverObject->DriverUnload = DriverUnload;

    DbgPrint("RaceConditionDrv loaded\n");
    return STATUS_SUCCESS;
}
