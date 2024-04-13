static void recalibrate_floppy(void)
{
	debugt(__func__, "");
	do_floppy = recal_interrupt;
	output_byte(FD_RECALIBRATE);
	if (output_byte(UNIT(current_drive)) < 0)
		reset_fdc();
}