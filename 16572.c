static struct dentry *proc_map_files_lookup(struct inode *dir,
		struct dentry *dentry, struct nameidata *nd)
{
	unsigned long vm_start, vm_end;
	struct vm_area_struct *vma;
	struct task_struct *task;
	struct dentry *result;
	struct mm_struct *mm;

	result = ERR_PTR(-EACCES);
	if (!capable(CAP_SYS_ADMIN))
		goto out;

	result = ERR_PTR(-ENOENT);
	task = get_proc_task(dir);
	if (!task)
		goto out;

	result = ERR_PTR(-EACCES);
	if (lock_trace(task))
		goto out_put_task;

	result = ERR_PTR(-ENOENT);
	if (dname_to_vma_addr(dentry, &vm_start, &vm_end))
		goto out_unlock;

	mm = get_task_mm(task);
	if (!mm)
		goto out_unlock;

	down_read(&mm->mmap_sem);
	vma = find_exact_vma(mm, vm_start, vm_end);
	if (!vma)
		goto out_no_vma;

	result = proc_map_files_instantiate(dir, dentry, task, vma->vm_file);

out_no_vma:
	up_read(&mm->mmap_sem);
	mmput(mm);
out_unlock:
	unlock_trace(task);
out_put_task:
	put_task_struct(task);
out:
	return result;
}