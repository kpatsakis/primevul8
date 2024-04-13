void display_sa_file_version(FILE *st, struct file_magic *file_magic)
{
	fprintf(st, _("File created by sar/sadc from sysstat version %d.%d.%d"),
		file_magic->sysstat_version,
		file_magic->sysstat_patchlevel,
		file_magic->sysstat_sublevel);

	if (file_magic->sysstat_extraversion) {
		fprintf(st, ".%d", file_magic->sysstat_extraversion);
	}
	fprintf(st, "\n");
}