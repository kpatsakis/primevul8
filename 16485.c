static int proc_readfdinfo(struct file *filp, void *dirent, filldir_t filldir)
{
	return proc_readfd_common(filp, dirent, filldir,
				  proc_fdinfo_instantiate);
}