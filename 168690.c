static UINT printer_process_irp_device_control(PRINTER_DEVICE* printer_dev, IRP* irp)
{
	Stream_Write_UINT32(irp->output, 0); /* OutputBufferLength */
	return irp->Complete(irp);
}