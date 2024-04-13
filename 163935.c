xfs_attr3_leaf_getvalue(
	struct xfs_buf		*bp,
	struct xfs_da_args	*args)
{
	struct xfs_attr_leafblock *leaf;
	struct xfs_attr3_icleaf_hdr ichdr;
	struct xfs_attr_leaf_entry *entry;
	struct xfs_attr_leaf_name_local *name_loc;
	struct xfs_attr_leaf_name_remote *name_rmt;

	leaf = bp->b_addr;
	xfs_attr3_leaf_hdr_from_disk(args->geo, &ichdr, leaf);
	ASSERT(ichdr.count < args->geo->blksize / 8);
	ASSERT(args->index < ichdr.count);

	entry = &xfs_attr3_leaf_entryp(leaf)[args->index];
	if (entry->flags & XFS_ATTR_LOCAL) {
		name_loc = xfs_attr3_leaf_name_local(leaf, args->index);
		ASSERT(name_loc->namelen == args->namelen);
		ASSERT(memcmp(args->name, name_loc->nameval, args->namelen) == 0);
		return xfs_attr_copy_value(args,
					&name_loc->nameval[args->namelen],
					be16_to_cpu(name_loc->valuelen));
	}

	name_rmt = xfs_attr3_leaf_name_remote(leaf, args->index);
	ASSERT(name_rmt->namelen == args->namelen);
	ASSERT(memcmp(args->name, name_rmt->name, args->namelen) == 0);
	args->rmtvaluelen = be32_to_cpu(name_rmt->valuelen);
	args->rmtblkno = be32_to_cpu(name_rmt->valueblk);
	args->rmtblkcnt = xfs_attr3_rmt_blocks(args->dp->i_mount,
					       args->rmtvaluelen);
	return xfs_attr_copy_value(args, NULL, args->rmtvaluelen);
}