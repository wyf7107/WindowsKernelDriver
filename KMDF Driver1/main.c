#include <ntddk.h>

#define add_code CTL_CODE(FILE_DEVICE_UNKNOWN, 	0x0701, 	METHOD_BUFFERED,FILE_ANY_ACCESS)



void UnLoadDriver(PDRIVER_OBJECT pDriver);
NTSTATUS CreateMyDevice(IN PDRIVER_OBJECT pDriverObject);
NTSTATUS CreateCall(PDEVICE_OBJECT pDevObj, PIRP irp);
NTSTATUS CloseCall(PDEVICE_OBJECT pDevObj, PIRP irp);
NTSTATUS IoControl(PDEVICE_OBJECT pDevObj, PIRP irp);



NTSTATUS CreateMyDevice(IN PDRIVER_OBJECT pDriverObject) {
	NTSTATUS status;
	PDEVICE_OBJECT pDevObj;

	UNICODE_STRING devName;
	UNICODE_STRING symLinkName;

	RtlInitUnicodeString(&devName, L"\\Device\\Yifan_Device");

	status = IoCreateDevice(pDriverObject, 0, &devName, FILE_DEVICE_UNKNOWN, 0, TRUE, &pDevObj);
	DbgPrint("Create Device Value: %x", status);
	if (!NT_SUCCESS(status)) //判断上面用的命令 IoCreateDevice创建设备返回的值
	{
		if (status == STATUS_INSUFFICIENT_RESOURCES) //相当于易语言 如果
		{
			DbgPrint("Insufficient resources....");

		}
		if (status == STATUS_OBJECT_NAME_EXISTS)  //相当于易语言 如果
		{
			DbgPrint("status object name exists...");

		}
		if (status == STATUS_OBJECT_NAME_COLLISION)  //相当于易语言 如果
		{
			DbgPrint("name collision...");

		}
		DbgPrint("failed creating new device...");

		return status;
	}
	DbgPrint("Succeffully created device!");

	pDevObj->Flags |= DO_BUFFERED_IO;

	RtlInitUnicodeString(&symLinkName, L"\\??\\Yifan_Com");

	status = IoCreateSymbolicLink(&symLinkName, &devName);
	DbgPrint("sym link name status: ", status);
	if (!NT_SUCCESS(status)) //如果status不等于0 就执行结构里的内容。
	{
		IoDeleteDevice(pDevObj); 
		DbgPrint("delete device success");//调用命令IoDeleteDevice删除当前pDevObj设备。
		return status;
		
	}
	DbgPrint("sucess creating symlink");
	return STATUS_SUCCESS;

}



NTSTATUS IoControl(PDEVICE_OBJECT pDevObj, PIRP irp) {
	NTSTATUS status;
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);
	ULONG ControlCode = stack->Parameters.DeviceIoControl.IoControlCode;

	

	if (ControlCode == add_code) {
		char pData[1024] = "eeee";
		PVOID pInputBuffer = irp->AssociatedIrp.SystemBuffer;
		DbgPrint("Message received: %s", pInputBuffer);
		PVOID pOutputBuffer = irp->AssociatedIrp.SystemBuffer;
		RtlCopyMemory(pOutputBuffer, pData, strlen(pData));
		status = STATUS_SUCCESS;
	}
	else {
		status = STATUS_INVALID_PARAMETER;
	}

	irp->IoStatus.Status = status;
	irp->IoStatus.Information = 1024;
	IoCompleteRequest(irp, IO_NO_INCREMENT);

	return status;

}


VOID CreateProcessNotifyEx(PEPROCESS Process, HANDLE ProcessId, PPS_CREATE_NOTIFY_INFO CreateInfo) {
	if (CreateInfo != NULL)
	{
		
		
	}

}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING str) {

	PsSetCreateProcessNotifyRoutineEx(CreateProcessNotifyEx, FALSE);

	CreateMyDevice(pDriver);

	pDriver->MajorFunction[IRP_MJ_CREATE] = CreateCall;
	pDriver->MajorFunction[IRP_MJ_CLOSE] = CloseCall;
	pDriver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoControl;



	pDriver->DriverUnload = UnLoadDriver;

	DbgPrint("Loaded Driver.");

	return 1;
}

void UnLoadDriver(PDRIVER_OBJECT pDriver) {

	PsSetCreateProcessNotifyRoutineEx(CreateProcessNotifyEx, TRUE);

	PDEVICE_OBJECT pDev;
	UNICODE_STRING symLinkName;

	pDev = pDriver->DeviceObject;
	IoDeleteDevice(pDev);
	RtlInitUnicodeString(&symLinkName, L"\\??\\Yifan_Com");
	IoDeleteSymbolicLink(&symLinkName);

	DbgPrint("Delete SymLinkName Success \n");
	DbgPrint("UnLoad Driver..");
	
}

NTSTATUS CreateCall(PDEVICE_OBJECT pDevObj, PIRP irp) {
	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;
	DbgPrint("IRP_MJ_CREATE run success!");
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS CloseCall(PDEVICE_OBJECT pDevObj, PIRP irp) {
	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;
	DbgPrint("IRP_MJ_CLOSE run success!");
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

