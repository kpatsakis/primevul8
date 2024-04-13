xfs_attr3_leaf_verify(
	struct xfs_buf			*bp)
{
	struct xfs_attr3_icleaf_hdr	ichdr;
	struct xfs_mount		*mp = bp->b_mount;
	struct xfs_attr_leafblock	*leaf = bp->b_addr;
	struct xfs_attr_leaf_entry	*entries;
	struct xfs_attr_leaf_entry	*ent;
	char				*buf_end;
	uint32_t			end;	/* must be 32bit - see below */
	__u32				last_hashval = 0;
	int				i;
	xfs_failaddr_t			fa;

	xfs_attr3_leaf_hdr_from_disk(mp->m_attr_geo, &ichdr, leaf);

	fa = xfs_da3_blkinfo_verify(bp, bp->b_addr);
	if (fa)
		return fa;

	/*
	 * firstused is the block offset of the first name info structure.
	 * Make sure it doesn't go off the block or crash into the header.
	 */
	if (ichdr.firstused > mp->m_attr_geo->blksize)
		return __this_address;
	if (ichdr.firstused < xfs_attr3_leaf_hdr_size(leaf))
		return __this_address;

	/* Make sure the entries array doesn't crash into the name info. */
	entries = xfs_attr3_leaf_entryp(bp->b_addr);
	if ((char *)&entries[ichdr.count] >
	    (char *)bp->b_addr + ichdr.firstused)
		return __this_address;

	/*
	 * NOTE: This verifier historically failed empty leaf buffers because
	 * we expect the fork to be in another format. Empty attr fork format
	 * conversions are possible during xattr set, however, and format
	 * conversion is not atomic with the xattr set that triggers it. We
	 * cannot assume leaf blocks are non-empty until that is addressed.
	*/
	buf_end = (char *)bp->b_addr + mp->m_attr_geo->blksize;
	for (i = 0, ent = entries; i < ichdr.count; ent++, i++) {
		fa = xfs_attr3_leaf_verify_entry(mp, buf_end, leaf, &ichdr,
				ent, i, &last_hashval);
		if (fa)
			return fa;
	}

	/*
	 * Quickly check the freemap information.  Attribute data has to be
	 * aligned to 4-byte boundaries, and likewise for the free space.
	 *
	 * Note that for 64k block size filesystems, the freemap entries cannot
	 * overflow as they are only be16 fields. However, when checking end
	 * pointer of the freemap, we have to be careful to detect overflows and
	 * so use uint32_t for those checks.
	 */
	for (i = 0; i < XFS_ATTR_LEAF_MAPSIZE; i++) {
		if (ichdr.freemap[i].base > mp->m_attr_geo->blksize)
			return __this_address;
		if (ichdr.freemap[i].base & 0x3)
			return __this_address;
		if (ichdr.freemap[i].size > mp->m_attr_geo->blksize)
			return __this_address;
		if (ichdr.freemap[i].size & 0x3)
			return __this_address;

		/* be care of 16 bit overflows here */
		end = (uint32_t)ichdr.freemap[i].base + ichdr.freemap[i].size;
		if (end < ichdr.freemap[i].base)
			return __this_address;
		if (end > mp->m_attr_geo->blksize)
			return __this_address;
	}

	return NULL;
}