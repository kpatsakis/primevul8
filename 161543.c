static void *timer_list_start(struct seq_file *file, loff_t *offset)
{
	struct timer_list_iter *iter = file->private;

	if (!*offset)
		iter->now = ktime_to_ns(ktime_get());
	iter->cpu = -1;
	iter->second_pass = false;
	return move_iter(iter, *offset);
}