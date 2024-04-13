static unsigned ext4_num_overhead_clusters(struct super_block *sb,
					   ext4_group_t block_group,
					   struct ext4_group_desc *gdp)
{
	unsigned num_clusters;
	int block_cluster = -1, inode_cluster = -1, itbl_cluster = -1, i, c;
	ext4_fsblk_t start = ext4_group_first_block_no(sb, block_group);
	ext4_fsblk_t itbl_blk;
	struct ext4_sb_info *sbi = EXT4_SB(sb);

	/* This is the number of clusters used by the superblock,
	 * block group descriptors, and reserved block group
	 * descriptor blocks */
	num_clusters = ext4_num_base_meta_clusters(sb, block_group);

	/*
	 * For the allocation bitmaps and inode table, we first need
	 * to check to see if the block is in the block group.  If it
	 * is, then check to see if the cluster is already accounted
	 * for in the clusters used for the base metadata cluster, or
	 * if we can increment the base metadata cluster to include
	 * that block.  Otherwise, we will have to track the cluster
	 * used for the allocation bitmap or inode table explicitly.
	 * Normally all of these blocks are contiguous, so the special
	 * case handling shouldn't be necessary except for *very*
	 * unusual file system layouts.
	 */
	if (ext4_block_in_group(sb, ext4_block_bitmap(sb, gdp), block_group)) {
		block_cluster = EXT4_B2C(sbi,
					 ext4_block_bitmap(sb, gdp) - start);
		if (block_cluster < num_clusters)
			block_cluster = -1;
		else if (block_cluster == num_clusters) {
			num_clusters++;
			block_cluster = -1;
		}
	}

	if (ext4_block_in_group(sb, ext4_inode_bitmap(sb, gdp), block_group)) {
		inode_cluster = EXT4_B2C(sbi,
					 ext4_inode_bitmap(sb, gdp) - start);
		if (inode_cluster < num_clusters)
			inode_cluster = -1;
		else if (inode_cluster == num_clusters) {
			num_clusters++;
			inode_cluster = -1;
		}
	}

	itbl_blk = ext4_inode_table(sb, gdp);
	for (i = 0; i < sbi->s_itb_per_group; i++) {
		if (ext4_block_in_group(sb, itbl_blk + i, block_group)) {
			c = EXT4_B2C(sbi, itbl_blk + i - start);
			if ((c < num_clusters) || (c == inode_cluster) ||
			    (c == block_cluster) || (c == itbl_cluster))
				continue;
			if (c == num_clusters) {
				num_clusters++;
				continue;
			}
			num_clusters++;
			itbl_cluster = c;
		}
	}

	if (block_cluster != -1)
		num_clusters++;
	if (inode_cluster != -1)
		num_clusters++;

	return num_clusters;
}