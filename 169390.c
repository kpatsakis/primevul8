static int cmp_user(struct List *i1, struct List *i2)
{
	PgUser *u1 = container_of(i1, PgUser, head);
	PgUser *u2 = container_of(i2, PgUser, head);
	return strcmp(u1->name, u2->name);
}