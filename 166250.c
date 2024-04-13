int ext4_bg_has_super(struct super_block *sb, ext4_group_t group)
{
	struct ext4_super_block *es = EXT4_SB(sb)->s_es;

	if (group == 0)
		return 1;
	if (ext4_has_feature_sparse_super2(sb)) {
		if (group == le32_to_cpu(es->s_backup_bgs[0]) ||
		    group == le32_to_cpu(es->s_backup_bgs[1]))
			return 1;
		return 0;
	}
	if ((group <= 1) || !ext4_has_feature_sparse_super(sb))
		return 1;
	if (!(group & 1))
		return 0;
	if (test_root(group, 3) || (test_root(group, 5)) ||
	    test_root(group, 7))
		return 1;

	return 0;
}