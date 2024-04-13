static void selinux_inode_getsecid(const struct inode *inode, u32 *secid)
{
	struct inode_security_struct *isec = inode->i_security;
	*secid = isec->sid;
}