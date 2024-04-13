proc_map_files_readdir(struct file *filp, void *dirent, filldir_t filldir)
{
	struct dentry *dentry = filp->f_path.dentry;
	struct inode *inode = dentry->d_inode;
	struct vm_area_struct *vma;
	struct task_struct *task;
	struct mm_struct *mm;
	ino_t ino;
	int ret;

	ret = -EACCES;
	if (!capable(CAP_SYS_ADMIN))
		goto out;

	ret = -ENOENT;
	task = get_proc_task(inode);
	if (!task)
		goto out;

	ret = -EACCES;
	if (lock_trace(task))
		goto out_put_task;

	ret = 0;
	switch (filp->f_pos) {
	case 0:
		ino = inode->i_ino;
		if (filldir(dirent, ".", 1, 0, ino, DT_DIR) < 0)
			goto out_unlock;
		filp->f_pos++;
	case 1:
		ino = parent_ino(dentry);
		if (filldir(dirent, "..", 2, 1, ino, DT_DIR) < 0)
			goto out_unlock;
		filp->f_pos++;
	default:
	{
		unsigned long nr_files, pos, i;
		struct flex_array *fa = NULL;
		struct map_files_info info;
		struct map_files_info *p;

		mm = get_task_mm(task);
		if (!mm)
			goto out_unlock;
		down_read(&mm->mmap_sem);

		nr_files = 0;

		/*
		 * We need two passes here:
		 *
		 *  1) Collect vmas of mapped files with mmap_sem taken
		 *  2) Release mmap_sem and instantiate entries
		 *
		 * otherwise we get lockdep complained, since filldir()
		 * routine might require mmap_sem taken in might_fault().
		 */

		for (vma = mm->mmap, pos = 2; vma; vma = vma->vm_next) {
			if (vma->vm_file && ++pos > filp->f_pos)
				nr_files++;
		}

		if (nr_files) {
			fa = flex_array_alloc(sizeof(info), nr_files,
						GFP_KERNEL);
			if (!fa || flex_array_prealloc(fa, 0, nr_files,
							GFP_KERNEL)) {
				ret = -ENOMEM;
				if (fa)
					flex_array_free(fa);
				up_read(&mm->mmap_sem);
				mmput(mm);
				goto out_unlock;
			}
			for (i = 0, vma = mm->mmap, pos = 2; vma;
					vma = vma->vm_next) {
				if (!vma->vm_file)
					continue;
				if (++pos <= filp->f_pos)
					continue;

				get_file(vma->vm_file);
				info.file = vma->vm_file;
				info.len = snprintf(info.name,
						sizeof(info.name), "%lx-%lx",
						vma->vm_start, vma->vm_end);
				if (flex_array_put(fa, i++, &info, GFP_KERNEL))
					BUG();
			}
		}
		up_read(&mm->mmap_sem);

		for (i = 0; i < nr_files; i++) {
			p = flex_array_get(fa, i);
			ret = proc_fill_cache(filp, dirent, filldir,
					      p->name, p->len,
					      proc_map_files_instantiate,
					      task, p->file);
			if (ret)
				break;
			filp->f_pos++;
			fput(p->file);
		}
		for (; i < nr_files; i++) {
			/*
			 * In case of error don't forget
			 * to put rest of file refs.
			 */
			p = flex_array_get(fa, i);
			fput(p->file);
		}
		if (fa)
			flex_array_free(fa);
		mmput(mm);
	}
	}

out_unlock:
	unlock_trace(task);
out_put_task:
	put_task_struct(task);
out:
	return ret;
}