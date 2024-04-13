ptvcursor_free(ptvcursor_t *ptvc)
{
	ptvcursor_free_subtree_levels(ptvc);
	/*g_free(ptvc);*/
}