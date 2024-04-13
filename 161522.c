static void *timer_list_next(struct seq_file *file, void *v, loff_t *offset)
{
	struct timer_list_iter *iter = file->private;
	++*offset;
	return move_iter(iter, 1);
}