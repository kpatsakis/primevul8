monitor_child_preauth(struct monitor *pmonitor)
{
	struct mon_table *ent;
	int authenticated = 0;

	debug3("preauth child monitor started");

	if (compat20) {
		mon_dispatch = mon_dispatch_proto20;

		/* Permit requests for moduli and signatures */
		monitor_permit(mon_dispatch, MONITOR_REQ_MODULI, 1);
		monitor_permit(mon_dispatch, MONITOR_REQ_SIGN, 1);
	} else {
		mon_dispatch = mon_dispatch_proto15;

		monitor_permit(mon_dispatch, MONITOR_REQ_SESSKEY, 1);
	}

	authctxt = authctxt_new();

	/* The first few requests do not require asynchronous access */
	while (!authenticated) {
		authenticated = monitor_read(pmonitor, mon_dispatch, &ent);
		if (authenticated) {
			if (!(ent->flags & MON_AUTHDECIDE))
				fatal("%s: unexpected authentication from %d",
				    __func__, ent->type);
			if (authctxt->pw->pw_uid == 0 &&
			    !auth_root_allowed(auth_method))
				authenticated = 0;
		}

		if (ent->flags & MON_AUTHDECIDE) {
			auth_log(authctxt, authenticated, auth_method,
			    compat20 ? " ssh2" : "");
			if (!authenticated)
				authctxt->failures++;
		}
	}

	if (!authctxt->valid)
		fatal("%s: authenticated invalid user", __func__);

	debug("%s: %s has been authenticated by privileged process",
	    __func__, authctxt->user);

	mm_get_keystate(pmonitor);

	return (authctxt);
}