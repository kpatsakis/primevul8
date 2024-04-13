unsigned long ext4_bg_num_gdb(struct super_block *sb, ext4_group_t group)
{
	unsigned long first_meta_bg =
			le32_to_cpu(EXT4_SB(sb)->s_es->s_first_meta_bg);
	unsigned long metagroup = group / EXT4_DESC_PER_BLOCK(sb);

	if (!ext4_has_feature_meta_bg(sb) || metagroup < first_meta_bg)
		return ext4_bg_num_gdb_nometa(sb, group);

	return ext4_bg_num_gdb_meta(sb,group);

}