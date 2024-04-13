isofile_init_hardlinks(struct iso9660 *iso9660)
{
	static const struct archive_rb_tree_ops rb_ops = {
		isofile_hd_cmp_node, isofile_hd_cmp_key,
	};

	__archive_rb_tree_init(&(iso9660->hardlink_rbtree), &rb_ops);
}