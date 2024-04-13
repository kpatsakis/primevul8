isofile_free_hardlinks(struct iso9660 *iso9660)
{
	struct archive_rb_node *n, *next;

	for (n = ARCHIVE_RB_TREE_MIN(&(iso9660->hardlink_rbtree)); n;) {
		next = __archive_rb_tree_iterate(&(iso9660->hardlink_rbtree),
		    n, ARCHIVE_RB_DIR_RIGHT);
		free(n);
		n = next;
	}
}