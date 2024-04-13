static gboolean compare_path(GtkTreeStore *store,
					GtkTreeIter *iter, gpointer user_data)
{
	const gchar *path = user_data;
	GDBusProxy *object;
	gboolean found = FALSE;

	gtk_tree_model_get(GTK_TREE_MODEL(store), iter,
					BLUETOOTH_COLUMN_PROXY, &object, -1);

	if (object != NULL) {
		found = g_str_equal(path, g_dbus_proxy_get_object_path(object));
		g_object_unref(object);
	}

	return found;
}