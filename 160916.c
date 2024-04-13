get_iter_from_address (GtkTreeStore *store,
		       GtkTreeIter  *iter,
		       const char   *address,
		       GDBusProxy   *adapter)
{
	GtkTreeIter parent_iter;

	if (get_iter_from_proxy (store, &parent_iter, adapter) == FALSE)
		return FALSE;

	return iter_search (store, iter, &parent_iter, compare_address, (gpointer) address);
}