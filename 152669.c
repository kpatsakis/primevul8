void which_sadc(void)
{
	struct stat buf;

	if (stat(SADC_PATH, &buf) < 0) {
		printf(_("Data collector will be sought in PATH\n"));
	}
	else {
		printf(_("Data collector found: %s\n"), SADC_PATH);
	}
	exit(0);
}