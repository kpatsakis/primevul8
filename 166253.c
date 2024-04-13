ext4_group_t ext4_get_group_number(struct super_block *sb,
				   ext4_fsblk_t block)
{
	ext4_group_t group;

	if (test_opt2(sb, STD_GROUP_SIZE))
		group = (block -
			 le32_to_cpu(EXT4_SB(sb)->s_es->s_first_data_block)) >>
			(EXT4_BLOCK_SIZE_BITS(sb) + EXT4_CLUSTER_BITS(sb) + 3);
	else
		ext4_get_group_no_and_offset(sb, block, &group, NULL);
	return group;
}