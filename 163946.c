xfs_attr3_leaf_verify_entry(
	struct xfs_mount			*mp,
	char					*buf_end,
	struct xfs_attr_leafblock		*leaf,
	struct xfs_attr3_icleaf_hdr		*leafhdr,
	struct xfs_attr_leaf_entry		*ent,
	int					idx,
	__u32					*last_hashval)
{
	struct xfs_attr_leaf_name_local		*lentry;
	struct xfs_attr_leaf_name_remote	*rentry;
	char					*name_end;
	unsigned int				nameidx;
	unsigned int				namesize;
	__u32					hashval;

	/* hash order check */
	hashval = be32_to_cpu(ent->hashval);
	if (hashval < *last_hashval)
		return __this_address;
	*last_hashval = hashval;

	nameidx = be16_to_cpu(ent->nameidx);
	if (nameidx < leafhdr->firstused || nameidx >= mp->m_attr_geo->blksize)
		return __this_address;

	/*
	 * Check the name information.  The namelen fields are u8 so we can't
	 * possibly exceed the maximum name length of 255 bytes.
	 */
	if (ent->flags & XFS_ATTR_LOCAL) {
		lentry = xfs_attr3_leaf_name_local(leaf, idx);
		namesize = xfs_attr_leaf_entsize_local(lentry->namelen,
				be16_to_cpu(lentry->valuelen));
		name_end = (char *)lentry + namesize;
		if (lentry->namelen == 0)
			return __this_address;
	} else {
		rentry = xfs_attr3_leaf_name_remote(leaf, idx);
		namesize = xfs_attr_leaf_entsize_remote(rentry->namelen);
		name_end = (char *)rentry + namesize;
		if (rentry->namelen == 0)
			return __this_address;
		if (!(ent->flags & XFS_ATTR_INCOMPLETE) &&
		    rentry->valueblk == 0)
			return __this_address;
	}

	if (name_end > buf_end)
		return __this_address;

	return NULL;
}