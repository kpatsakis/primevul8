static int proc_fd_info(struct inode *inode, struct path *path, char *info)
{
	struct task_struct *task = get_proc_task(inode);
	struct files_struct *files = NULL;
	struct file *file;
	int fd = proc_fd(inode);

	if (task) {
		files = get_files_struct(task);
		put_task_struct(task);
	}
	if (files) {
		/*
		 * We are not taking a ref to the file structure, so we must
		 * hold ->file_lock.
		 */
		spin_lock(&files->file_lock);
		file = fcheck_files(files, fd);
		if (file) {
			unsigned int f_flags;
			struct fdtable *fdt;

			fdt = files_fdtable(files);
			f_flags = file->f_flags & ~O_CLOEXEC;
			if (FD_ISSET(fd, fdt->close_on_exec))
				f_flags |= O_CLOEXEC;

			if (path) {
				*path = file->f_path;
				path_get(&file->f_path);
			}
			if (info)
				snprintf(info, PROC_FDINFO_MAX,
					 "pos:\t%lli\n"
					 "flags:\t0%o\n",
					 (long long) file->f_pos,
					 f_flags);
			spin_unlock(&files->file_lock);
			put_files_struct(files);
			return 0;
		}
		spin_unlock(&files->file_lock);
		put_files_struct(files);
	}
	return -ENOENT;
}