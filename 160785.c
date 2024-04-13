static int conflict_entry_name(
	git_buf *out,
	const char *side_name,
	const char *filename)
{
	if (git_buf_puts(out, side_name) < 0 ||
		git_buf_putc(out, ':') < 0 ||
		git_buf_puts(out, filename) < 0)
		return -1;

	return 0;
}