__print_funct_t print_sar_comment(int *tab, int action, char *cur_date, char *cur_time, int utc,
				  char *comment, struct file_header *file_hdr)
{
	printf("%-11s", cur_time);
	cprintf_s(IS_COMMENT, "  COM %s\n", comment);
}