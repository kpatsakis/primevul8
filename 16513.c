static int proc_pid_fill_cache(struct file *filp, void *dirent, filldir_t filldir,
	struct tgid_iter iter)
{
	char name[PROC_NUMBUF];
	int len = snprintf(name, sizeof(name), "%d", iter.tgid);
	return proc_fill_cache(filp, dirent, filldir, name, len,
				proc_pid_instantiate, iter.task, NULL);
}