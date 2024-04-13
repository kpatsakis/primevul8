
static void io_free_page_table(void **table, size_t size)
{
	unsigned i, nr_tables = DIV_ROUND_UP(size, PAGE_SIZE);

	for (i = 0; i < nr_tables; i++)
		kfree(table[i]);
	kfree(table);