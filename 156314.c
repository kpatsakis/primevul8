monitor_child_postauth(struct monitor *pmonitor)
{
	if (compat20) {
		mon_dispatch = mon_dispatch_postauth20;

		/* Permit requests for moduli and signatures */
		monitor_permit(mon_dispatch, MONITOR_REQ_MODULI, 1);
		monitor_permit(mon_dispatch, MONITOR_REQ_SIGN, 1);
		monitor_permit(mon_dispatch, MONITOR_REQ_TERM, 1);

	} else {
		mon_dispatch = mon_dispatch_postauth15;
		monitor_permit(mon_dispatch, MONITOR_REQ_TERM, 1);
	}
	if (!no_pty_flag) {
		monitor_permit(mon_dispatch, MONITOR_REQ_PTY, 1);
		monitor_permit(mon_dispatch, MONITOR_REQ_PTYCLEANUP, 1);
	}

	for (;;)
		monitor_read(pmonitor, mon_dispatch, NULL);
}