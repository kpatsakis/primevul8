path_table_add_entry(struct path_table *pathtbl, struct isoent *ent)
{
	ent->ptnext = NULL;
	*pathtbl->last = ent;
	pathtbl->last = &(ent->ptnext);
	pathtbl->cnt ++;
}