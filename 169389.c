static int cmp_pool(struct List *i1, struct List *i2)
{
	PgPool *p1 = container_of(i1, PgPool, head);
	PgPool *p2 = container_of(i2, PgPool, head);
	if (p1->db != p2->db)
		return strcmp(p1->db->name, p2->db->name);
	if (p1->user != p2->user)
		return strcmp(p1->user->name, p2->user->name);
	return 0;
}