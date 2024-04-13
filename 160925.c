compare_address (GtkTreeStore *store,
		 GtkTreeIter *iter,
		 gpointer user_data)
{
	const char *address = user_data;
	char *tmp_address;
	gboolean found = FALSE;

	gtk_tree_model_get (GTK_TREE_MODEL(store), iter,
			    BLUETOOTH_COLUMN_ADDRESS, &tmp_address, -1);
	found = g_str_equal (address, tmp_address);
	g_free (tmp_address);

	return found;
}