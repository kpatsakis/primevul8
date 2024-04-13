static int nfs4_setlease(struct file *file, long arg, struct file_lock **lease,
			 void **priv)
{
	return nfs4_proc_setlease(file, arg, lease, priv);
}