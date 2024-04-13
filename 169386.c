PgUser *add_user(const char *name, const char *passwd)
{
	PgUser *user = find_user(name);

	if (user == NULL) {
		user = slab_alloc(user_cache);
		if (!user)
			return NULL;

		list_init(&user->head);
		list_init(&user->pool_list);
		safe_strcpy(user->name, name, sizeof(user->name));
		put_in_order(&user->head, &user_list, cmp_user);

		aatree_insert(&user_tree, (uintptr_t)user->name, &user->tree_node);
	}
	safe_strcpy(user->passwd, passwd, sizeof(user->passwd));
	return user;
}