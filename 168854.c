static UINT printer_irp_request(DEVICE* device, IRP* irp)
{
	PRINTER_DEVICE* printer_dev = (PRINTER_DEVICE*)device;
	InterlockedPushEntrySList(printer_dev->pIrpList, &(irp->ItemEntry));
	SetEvent(printer_dev->event);
	return CHANNEL_RC_OK;
}