static int cmp_database(struct List *i1, struct List *i2)
{
	PgDatabase *db1 = container_of(i1, PgDatabase, head);
	PgDatabase *db2 = container_of(i2, PgDatabase, head);
	return strcmp(db1->name, db2->name);
}