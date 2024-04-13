
static void io_free_file_tables(struct io_file_table *table)
{
	kvfree(table->files);
	table->files = NULL;