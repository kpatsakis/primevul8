static int lstats_show_proc(struct seq_file *m, void *v)
{
	int i;
	struct inode *inode = m->private;
	struct task_struct *task = get_proc_task(inode);

	if (!task)
		return -ESRCH;
	seq_puts(m, "Latency Top version : v0.1\n");
	for (i = 0; i < 32; i++) {
		struct latency_record *lr = &task->latency_record[i];
		if (lr->backtrace[0]) {
			int q;
			seq_printf(m, "%i %li %li",
				   lr->count, lr->time, lr->max);
			for (q = 0; q < LT_BACKTRACEDEPTH; q++) {
				unsigned long bt = lr->backtrace[q];
				if (!bt)
					break;
				if (bt == ULONG_MAX)
					break;
				seq_printf(m, " %ps", (void *)bt);
			}
			seq_putc(m, '\n');
		}

	}
	put_task_struct(task);
	return 0;
}