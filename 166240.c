unsigned ext4_free_clusters_after_init(struct super_block *sb,
				       ext4_group_t block_group,
				       struct ext4_group_desc *gdp)
{
	return num_clusters_in_group(sb, block_group) - 
		ext4_num_overhead_clusters(sb, block_group, gdp);
}