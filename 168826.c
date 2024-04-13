static UINT printer_process_irp_close(PRINTER_DEVICE* printer_dev, IRP* irp)
{
	rdpPrintJob* printjob = NULL;

	if (printer_dev->printer)
		printjob = printer_dev->printer->FindPrintJob(printer_dev->printer, irp->FileId);

	if (!printjob)
	{
		irp->IoStatus = STATUS_UNSUCCESSFUL;
	}
	else
	{
		printjob->Close(printjob);
	}

	Stream_Zero(irp->output, 4); /* Padding(4) */
	return irp->Complete(irp);
}