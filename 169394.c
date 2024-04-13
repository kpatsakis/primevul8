PgUser *find_user(const char *name)
{
	PgUser *user = NULL;
	struct AANode *node;

	node = aatree_search(&user_tree, (uintptr_t)name);
	user = node ? container_of(node, PgUser, tree_node) : NULL;
	return user;
}