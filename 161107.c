isoent_find_child(struct isoent *isoent, const char *child_name)
{
	struct isoent *np;

	np = (struct isoent *)__archive_rb_tree_find_node(
	    &(isoent->rbtree), child_name);
	return (np);
}