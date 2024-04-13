static int map_files_d_revalidate(struct dentry *dentry, struct nameidata *nd)
{
	unsigned long vm_start, vm_end;
	bool exact_vma_exists = false;
	struct mm_struct *mm = NULL;
	struct task_struct *task;
	const struct cred *cred;
	struct inode *inode;
	int status = 0;

	if (nd && nd->flags & LOOKUP_RCU)
		return -ECHILD;

	if (!capable(CAP_SYS_ADMIN)) {
		status = -EACCES;
		goto out_notask;
	}

	inode = dentry->d_inode;
	task = get_proc_task(inode);
	if (!task)
		goto out_notask;

	if (!ptrace_may_access(task, PTRACE_MODE_READ))
		goto out;

	mm = get_task_mm(task);
	if (!mm)
		goto out;

	if (!dname_to_vma_addr(dentry, &vm_start, &vm_end)) {
		down_read(&mm->mmap_sem);
		exact_vma_exists = !!find_exact_vma(mm, vm_start, vm_end);
		up_read(&mm->mmap_sem);
	}

	mmput(mm);

	if (exact_vma_exists) {
		if (task_dumpable(task)) {
			rcu_read_lock();
			cred = __task_cred(task);
			inode->i_uid = cred->euid;
			inode->i_gid = cred->egid;
			rcu_read_unlock();
		} else {
			inode->i_uid = 0;
			inode->i_gid = 0;
		}
		security_task_to_inode(task, inode);
		status = 1;
	}

out:
	put_task_struct(task);

out_notask:
	if (status <= 0)
		d_drop(dentry);

	return status;
}