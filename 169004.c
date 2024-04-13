static void fn_dec_console(struct vc_data *vc)
{
	int i, cur = fg_console;

	/* Currently switching?  Queue this next switch relative to that. */
	if (want_console != -1)
		cur = want_console;

	for (i = cur - 1; i != cur; i--) {
		if (i == -1)
			i = MAX_NR_CONSOLES - 1;
		if (vc_cons_allocated(i))
			break;
	}
	set_console(i);
}