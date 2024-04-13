static int selinux_cmp_sb_context(const struct super_block *oldsb,
				    const struct super_block *newsb)
{
	struct superblock_security_struct *old = oldsb->s_security;
	struct superblock_security_struct *new = newsb->s_security;
	char oldflags = old->flags & SE_MNTMASK;
	char newflags = new->flags & SE_MNTMASK;

	if (oldflags != newflags)
		goto mismatch;
	if ((oldflags & FSCONTEXT_MNT) && old->sid != new->sid)
		goto mismatch;
	if ((oldflags & CONTEXT_MNT) && old->mntpoint_sid != new->mntpoint_sid)
		goto mismatch;
	if ((oldflags & DEFCONTEXT_MNT) && old->def_sid != new->def_sid)
		goto mismatch;
	if (oldflags & ROOTCONTEXT_MNT) {
		struct inode_security_struct *oldroot = oldsb->s_root->d_inode->i_security;
		struct inode_security_struct *newroot = newsb->s_root->d_inode->i_security;
		if (oldroot->sid != newroot->sid)
			goto mismatch;
	}
	return 0;
mismatch:
	printk(KERN_WARNING "SELinux: mount invalid.  Same superblock, "
			    "different security settings for (dev %s, "
			    "type %s)\n", newsb->s_id, newsb->s_type->name);
	return -EBUSY;
}