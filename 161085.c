path_table_last_entry(struct path_table *pathtbl)
{
	if (pathtbl->first == NULL)
		return (NULL);
	return (((struct isoent *)(void *)
		((char *)(pathtbl->last) - offsetof(struct isoent, ptnext))));
}