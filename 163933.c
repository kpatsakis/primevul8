xfs_attr3_leaf_lookup_int(
	struct xfs_buf		*bp,
	struct xfs_da_args	*args)
{
	struct xfs_attr_leafblock *leaf;
	struct xfs_attr3_icleaf_hdr ichdr;
	struct xfs_attr_leaf_entry *entry;
	struct xfs_attr_leaf_entry *entries;
	struct xfs_attr_leaf_name_local *name_loc;
	struct xfs_attr_leaf_name_remote *name_rmt;
	xfs_dahash_t		hashval;
	int			probe;
	int			span;

	trace_xfs_attr_leaf_lookup(args);

	leaf = bp->b_addr;
	xfs_attr3_leaf_hdr_from_disk(args->geo, &ichdr, leaf);
	entries = xfs_attr3_leaf_entryp(leaf);
	if (ichdr.count >= args->geo->blksize / 8) {
		xfs_buf_mark_corrupt(bp);
		return -EFSCORRUPTED;
	}

	/*
	 * Binary search.  (note: small blocks will skip this loop)
	 */
	hashval = args->hashval;
	probe = span = ichdr.count / 2;
	for (entry = &entries[probe]; span > 4; entry = &entries[probe]) {
		span /= 2;
		if (be32_to_cpu(entry->hashval) < hashval)
			probe += span;
		else if (be32_to_cpu(entry->hashval) > hashval)
			probe -= span;
		else
			break;
	}
	if (!(probe >= 0 && (!ichdr.count || probe < ichdr.count))) {
		xfs_buf_mark_corrupt(bp);
		return -EFSCORRUPTED;
	}
	if (!(span <= 4 || be32_to_cpu(entry->hashval) == hashval)) {
		xfs_buf_mark_corrupt(bp);
		return -EFSCORRUPTED;
	}

	/*
	 * Since we may have duplicate hashval's, find the first matching
	 * hashval in the leaf.
	 */
	while (probe > 0 && be32_to_cpu(entry->hashval) >= hashval) {
		entry--;
		probe--;
	}
	while (probe < ichdr.count &&
	       be32_to_cpu(entry->hashval) < hashval) {
		entry++;
		probe++;
	}
	if (probe == ichdr.count || be32_to_cpu(entry->hashval) != hashval) {
		args->index = probe;
		return -ENOATTR;
	}

	/*
	 * Duplicate keys may be present, so search all of them for a match.
	 */
	for (; probe < ichdr.count && (be32_to_cpu(entry->hashval) == hashval);
			entry++, probe++) {
/*
 * GROT: Add code to remove incomplete entries.
 */
		if (entry->flags & XFS_ATTR_LOCAL) {
			name_loc = xfs_attr3_leaf_name_local(leaf, probe);
			if (!xfs_attr_match(args, name_loc->namelen,
					name_loc->nameval, entry->flags))
				continue;
			args->index = probe;
			return -EEXIST;
		} else {
			name_rmt = xfs_attr3_leaf_name_remote(leaf, probe);
			if (!xfs_attr_match(args, name_rmt->namelen,
					name_rmt->name, entry->flags))
				continue;
			args->index = probe;
			args->rmtvaluelen = be32_to_cpu(name_rmt->valuelen);
			args->rmtblkno = be32_to_cpu(name_rmt->valueblk);
			args->rmtblkcnt = xfs_attr3_rmt_blocks(
							args->dp->i_mount,
							args->rmtvaluelen);
			return -EEXIST;
		}
	}
	args->index = probe;
	return -ENOATTR;
}