xfs_attr3_leaf_firstused_from_disk(
	struct xfs_da_geometry		*geo,
	struct xfs_attr3_icleaf_hdr	*to,
	struct xfs_attr_leafblock	*from)
{
	struct xfs_attr3_leaf_hdr	*hdr3;

	if (from->hdr.info.magic == cpu_to_be16(XFS_ATTR3_LEAF_MAGIC)) {
		hdr3 = (struct xfs_attr3_leaf_hdr *) from;
		to->firstused = be16_to_cpu(hdr3->firstused);
	} else {
		to->firstused = be16_to_cpu(from->hdr.firstused);
	}

	/*
	 * Convert from the magic fsb size value to actual blocksize. This
	 * should only occur for empty blocks when the block size overflows
	 * 16-bits.
	 */
	if (to->firstused == XFS_ATTR3_LEAF_NULLOFF) {
		ASSERT(!to->count && !to->usedbytes);
		ASSERT(geo->blksize > USHRT_MAX);
		to->firstused = geo->blksize;
	}
}