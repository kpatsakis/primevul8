xfs_attr_leaf_entsize(xfs_attr_leafblock_t *leaf, int index)
{
	struct xfs_attr_leaf_entry *entries;
	xfs_attr_leaf_name_local_t *name_loc;
	xfs_attr_leaf_name_remote_t *name_rmt;
	int size;

	entries = xfs_attr3_leaf_entryp(leaf);
	if (entries[index].flags & XFS_ATTR_LOCAL) {
		name_loc = xfs_attr3_leaf_name_local(leaf, index);
		size = xfs_attr_leaf_entsize_local(name_loc->namelen,
						   be16_to_cpu(name_loc->valuelen));
	} else {
		name_rmt = xfs_attr3_leaf_name_remote(leaf, index);
		size = xfs_attr_leaf_entsize_remote(name_rmt->namelen);
	}
	return size;
}