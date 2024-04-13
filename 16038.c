static int m_show(struct seq_file *m, void *v)
{
	struct proc_mounts *p = m->private;
	struct mount *r = v;
	return p->show(m, &r->mnt);
}