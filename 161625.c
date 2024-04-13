static void print_name_offset(struct seq_file *m, void *sym)
{
	char symname[KSYM_NAME_LEN];

	if (lookup_symbol_name((unsigned long)sym, symname) < 0)
		SEQ_printf(m, "<%pK>", sym);
	else
		SEQ_printf(m, "%s", symname);
}