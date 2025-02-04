static void SEQ_printf(struct seq_file *m, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);

	if (m)
		seq_vprintf(m, fmt, args);
	else
		vprintk(fmt, args);

	va_end(args);
}