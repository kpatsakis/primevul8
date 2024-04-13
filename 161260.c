int set_create_files_as(struct cred *new, struct inode *inode)
{
	new->fsuid = inode->i_uid;
	new->fsgid = inode->i_gid;
	return security_kernel_create_files_as(new, inode);
}