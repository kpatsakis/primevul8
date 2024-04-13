static void *m_start(struct seq_file *m, loff_t *pos)
{
	struct proc_mounts *p = m->private;
	struct list_head *prev;

	down_read(&namespace_sem);
	if (!*pos) {
		prev = &p->ns->list;
	} else {
		prev = &p->cursor.mnt_list;

		/* Read after we'd reached the end? */
		if (list_empty(prev))
			return NULL;
	}

	return mnt_list_next(p->ns, prev);
}