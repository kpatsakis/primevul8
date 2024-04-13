PgUser *force_user(PgDatabase *db, const char *name, const char *passwd)
{
	PgUser *user = db->forced_user;
	if (!user) {
		user = slab_alloc(user_cache);
		if (!user)
			return NULL;
		list_init(&user->head);
		list_init(&user->pool_list);
	}
	safe_strcpy(user->name, name, sizeof(user->name));
	safe_strcpy(user->passwd, passwd, sizeof(user->passwd));
	db->forced_user = user;
	return user;
}