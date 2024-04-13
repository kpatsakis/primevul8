get_iter_from_path (GtkTreeStore *store,
		    GtkTreeIter *iter,
		    const char *path)
{
	return iter_search(store, iter, NULL, compare_path, (gpointer) path);
}