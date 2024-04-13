PgDatabase *find_database(const char *name)
{
	struct List *item, *tmp;
	PgDatabase *db;
	statlist_for_each(item, &database_list) {
		db = container_of(item, PgDatabase, head);
		if (strcmp(db->name, name) == 0)
			return db;
	}
	/* also trying to find in idle autodatabases list */
	statlist_for_each_safe(item, &autodatabase_idle_list, tmp) {
		db = container_of(item, PgDatabase, head);
		if (strcmp(db->name, name) == 0) {
			db->inactive_time = 0;
			statlist_remove(&autodatabase_idle_list, &db->head);
			put_in_order(&db->head, &database_list, cmp_database);
			return db;
		}
	}
	return NULL;
}