static void selinux_task_to_inode(struct task_struct *p,
				  struct inode *inode)
{
	struct inode_security_struct *isec = inode->i_security;
	u32 sid = task_sid(p);

	isec->sid = sid;
	isec->initialized = 1;
}