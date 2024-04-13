set_sys_leap(u_char new_sys_leap) {
	sys_leap = new_sys_leap;
	xmt_leap = sys_leap;

	/*
	 * Under certain conditions we send faked leap bits to clients, so
	 * eventually change xmt_leap below, but never change LEAP_NOTINSYNC.
	 */
	if (xmt_leap != LEAP_NOTINSYNC) {
		if (leap_sec_in_progress) {
			/* always send "not sync" */
			xmt_leap = LEAP_NOTINSYNC;
		}
#ifdef LEAP_SMEAR
		else {
			/*
			 * If leap smear is enabled in general we must never send a leap second warning
			 * to clients, so make sure we only send "in sync".
			 */
			if (leap_smear.enabled)
				xmt_leap = LEAP_NOWARNING;
		}
#endif	/* LEAP_SMEAR */
	}
}