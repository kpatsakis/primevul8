static void cmm_smsg_target(const char *from, char *msg)
{
	long nr, seconds;

	if (strlen(sender) > 0 && strcmp(from, sender) != 0)
		return;
	if (!cmm_skip_blanks(msg + strlen(SMSG_PREFIX), &msg))
		return;
	if (strncmp(msg, "SHRINK", 6) == 0) {
		if (!cmm_skip_blanks(msg + 6, &msg))
			return;
		nr = simple_strtoul(msg, &msg, 0);
		cmm_skip_blanks(msg, &msg);
		if (*msg == '\0')
			cmm_set_pages(nr);
	} else if (strncmp(msg, "RELEASE", 7) == 0) {
		if (!cmm_skip_blanks(msg + 7, &msg))
			return;
		nr = simple_strtoul(msg, &msg, 0);
		cmm_skip_blanks(msg, &msg);
		if (*msg == '\0')
			cmm_add_timed_pages(nr);
	} else if (strncmp(msg, "REUSE", 5) == 0) {
		if (!cmm_skip_blanks(msg + 5, &msg))
			return;
		nr = simple_strtoul(msg, &msg, 0);
		if (!cmm_skip_blanks(msg, &msg))
			return;
		seconds = simple_strtoul(msg, &msg, 0);
		cmm_skip_blanks(msg, &msg);
		if (*msg == '\0')
			cmm_set_timeout(nr, seconds);
	}
}