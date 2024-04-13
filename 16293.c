
static void io_fixed_file_set(struct io_fixed_file *file_slot, struct file *file)
{
	unsigned long file_ptr = (unsigned long) file;

	file_ptr |= io_file_get_flags(file);
	file_slot->file_ptr = file_ptr;