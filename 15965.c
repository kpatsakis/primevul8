static int do_umount_root(struct super_block *sb)
{
	int ret = 0;

	down_write(&sb->s_umount);
	if (!sb_rdonly(sb)) {
		struct fs_context *fc;

		fc = fs_context_for_reconfigure(sb->s_root, SB_RDONLY,
						SB_RDONLY);
		if (IS_ERR(fc)) {
			ret = PTR_ERR(fc);
		} else {
			ret = parse_monolithic_mount_data(fc, NULL);
			if (!ret)
				ret = reconfigure_super(fc);
			put_fs_context(fc);
		}
	}
	up_write(&sb->s_umount);
	return ret;
}