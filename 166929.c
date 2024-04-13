ptvcursor_tree(ptvcursor_t *ptvc)
{
	if (!ptvc)
		return NULL;

	return ptvc->tree;
}