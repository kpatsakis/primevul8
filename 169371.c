static int user_node_cmp(uintptr_t userptr, struct AANode *node)
{
	const char *name = (const char *)userptr;
	PgUser *user = container_of(node, PgUser, tree_node);
	return strcmp(name, user->name);
}