isoent_add_child_tail(struct isoent *parent, struct isoent *child)
{

	if (!__archive_rb_tree_insert_node(
	    &(parent->rbtree), (struct archive_rb_node *)child))
		return (0);
	child->chnext = NULL;
	*parent->children.last = child;
	parent->children.last = &(child->chnext);
	parent->children.cnt++;
	child->parent = parent;

	/* Add a child to a sub-directory chain */
	child->drnext = NULL;
	if (child->dir) {
		*parent->subdirs.last = child;
		parent->subdirs.last = &(child->drnext);
		parent->subdirs.cnt++;
		child->parent = parent;
	}
	return (1);
}