static UINT printer_process_irp_create(PRINTER_DEVICE* printer_dev, IRP* irp)
{
	rdpPrintJob* printjob = NULL;

	if (printer_dev->printer)
		printjob =
		    printer_dev->printer->CreatePrintJob(printer_dev->printer, irp->devman->id_sequence++);

	if (printjob)
	{
		Stream_Write_UINT32(irp->output, printjob->id); /* FileId */
	}
	else
	{
		Stream_Write_UINT32(irp->output, 0); /* FileId */
		irp->IoStatus = STATUS_PRINT_QUEUE_FULL;
	}

	return irp->Complete(irp);
}