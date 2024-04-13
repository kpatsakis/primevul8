static void recal_interrupt(void)
{
	debugt(__func__, "");
	if (inr != 2)
		FDCS->reset = 1;
	else if (ST0 & ST0_ECE) {
		switch (DRS->track) {
		case NEED_1_RECAL:
			debugt(__func__, "need 1 recal");
			/* after a second recalibrate, we still haven't
			 * reached track 0. Probably no drive. Raise an
			 * error, as failing immediately might upset
			 * computers possessed by the Devil :-) */
			cont->error();
			cont->redo();
			return;
		case NEED_2_RECAL:
			debugt(__func__, "need 2 recal");
			/* If we already did a recalibrate,
			 * and we are not at track 0, this
			 * means we have moved. (The only way
			 * not to move at recalibration is to
			 * be already at track 0.) Clear the
			 * new change flag */
			debug_dcl(DP->flags,
				  "clearing NEWCHANGE flag because of second recalibrate\n");

			clear_bit(FD_DISK_NEWCHANGE_BIT, &DRS->flags);
			DRS->select_date = jiffies;
			/* fall through */
		default:
			debugt(__func__, "default");
			/* Recalibrate moves the head by at
			 * most 80 steps. If after one
			 * recalibrate we don't have reached
			 * track 0, this might mean that we
			 * started beyond track 80.  Try
			 * again.  */
			DRS->track = NEED_1_RECAL;
			break;
		}
	} else
		DRS->track = ST1;
	floppy_ready();
}