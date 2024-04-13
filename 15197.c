void fs_machineid(void) {
	union machineid_t {
		uint8_t u8[16];
		uint32_t u32[4];
	} mid;

	// if --machine-id flag is inactive, do nothing
	if (arg_machineid == 0)
		return;
	if (arg_debug)
		printf("Generating a new machine-id\n");

	// init random number generator
	srand(time(NULL));

	// generate random id
	mid.u32[0] = rand();
	mid.u32[1] = rand();
	mid.u32[2] = rand();
	mid.u32[3] = rand();

	// UUID version 4 and DCE variant
	mid.u8[6] = (mid.u8[6] & 0x0F) | 0x40;
	mid.u8[8] = (mid.u8[8] & 0x3F) | 0x80;

	// write it in a file
	FILE *fp = fopen(RUN_MACHINEID, "we");
	if (!fp)
		errExit("fopen");
	fprintf(fp, "%08x%08x%08x%08x\n", mid.u32[0], mid.u32[1], mid.u32[2], mid.u32[3]);
	fclose(fp);
	if (set_perms(RUN_MACHINEID, 0, 0, 0444))
		errExit("set_perms");

	selinux_relabel_path(RUN_MACHINEID, "/etc/machine-id");

	struct stat s;
	if (stat("/etc/machine-id", &s) == 0) {
		if (arg_debug)
			printf("installing a new /etc/machine-id\n");

		if (mount(RUN_MACHINEID, "/etc/machine-id", "none", MS_BIND, "mode=444,gid=0"))
			errExit("mount");
	}
	if (stat("/var/lib/dbus/machine-id", &s) == 0) {
		if (mount(RUN_MACHINEID, "/var/lib/dbus/machine-id", "none", MS_BIND, "mode=444,gid=0"))
			errExit("mount");
	}
}