
static inline struct io_fixed_file *io_fixed_file_slot(struct io_file_table *table,
						       unsigned i)
{
	return &table->files[i];