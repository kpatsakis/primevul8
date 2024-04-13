static void m_stop(struct seq_file *m, void *v)
{
	struct proc_mounts *p = m->private;
	struct mount *mnt = v;

	lock_ns_list(p->ns);
	if (mnt)
		list_move_tail(&p->cursor.mnt_list, &mnt->mnt_list);
	else
		list_del_init(&p->cursor.mnt_list);
	unlock_ns_list(p->ns);
	up_read(&namespace_sem);
}