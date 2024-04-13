ext4_fsblk_t ext4_new_meta_blocks(handle_t *handle, struct inode *inode,
				  ext4_fsblk_t goal, unsigned int flags,
				  unsigned long *count, int *errp)
{
	struct ext4_allocation_request ar;
	ext4_fsblk_t ret;

	memset(&ar, 0, sizeof(ar));
	/* Fill with neighbour allocated blocks */
	ar.inode = inode;
	ar.goal = goal;
	ar.len = count ? *count : 1;
	ar.flags = flags;

	ret = ext4_mb_new_blocks(handle, &ar, errp);
	if (count)
		*count = ar.len;
	/*
	 * Account for the allocated meta blocks.  We will never
	 * fail EDQUOT for metdata, but we do account for it.
	 */
	if (!(*errp) && (flags & EXT4_MB_DELALLOC_RESERVED)) {
		dquot_alloc_block_nofail(inode,
				EXT4_C2B(EXT4_SB(inode->i_sb), ar.len));
	}
	return ret;
}