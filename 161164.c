isoent_new(struct isofile *file)
{
	struct isoent *isoent;
	static const struct archive_rb_tree_ops rb_ops = {
		isoent_cmp_node, isoent_cmp_key,
	};

	isoent = calloc(1, sizeof(*isoent));
	if (isoent == NULL)
		return (NULL);
	isoent->file = file;
	isoent->children.first = NULL;
	isoent->children.last = &(isoent->children.first);
	__archive_rb_tree_init(&(isoent->rbtree), &rb_ops);
	isoent->subdirs.first = NULL;
	isoent->subdirs.last = &(isoent->subdirs.first);
	isoent->extr_rec_list.first = NULL;
	isoent->extr_rec_list.last = &(isoent->extr_rec_list.first);
	isoent->extr_rec_list.current = NULL;
	if (archive_entry_filetype(file->entry) == AE_IFDIR)
		isoent->dir = 1;

	return (isoent);
}