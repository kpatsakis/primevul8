static void *m_next(struct seq_file *m, void *v, loff_t *pos)
{
	struct proc_mounts *p = m->private;
	struct mount *mnt = v;

	++*pos;
	return mnt_list_next(p->ns, &mnt->mnt_list);
}