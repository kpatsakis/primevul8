isoent_add_child_head(struct isoent *parent, struct isoent *child)
{

	if (!__archive_rb_tree_insert_node(
	    &(parent->rbtree), (struct archive_rb_node *)child))
		return (0);
	if ((child->chnext = parent->children.first) == NULL)
		parent->children.last = &(child->chnext);
	parent->children.first = child;
	parent->children.cnt++;
	child->parent = parent;

	/* Add a child to a sub-directory chain */
	if (child->dir) {
		if ((child->drnext = parent->subdirs.first) == NULL)
			parent->subdirs.last = &(child->drnext);
		parent->subdirs.first = child;
		parent->subdirs.cnt++;
		child->parent = parent;
	} else
		child->drnext = NULL;
	return (1);
}