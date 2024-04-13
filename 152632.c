__printf_funct_t print_sar_restart(int *tab, int action, char *cur_date, char *cur_time,
				  int utc, struct file_header *file_hdr)
{
	char restart[64];

	printf("\n%-11s", cur_time);
	sprintf(restart, "  LINUX RESTART\t(%d CPU)\n",
		file_hdr->sa_cpu_nr > 1 ? file_hdr->sa_cpu_nr - 1 : 1);
	cprintf_s(IS_RESTART, "%s", restart);

}