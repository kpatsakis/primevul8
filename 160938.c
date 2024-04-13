adapter_notify_cb (Adapter1       *adapter,
		   GParamSpec     *pspec,
		   BluetoothClient *client)
{
	BluetoothClientPrivate *priv = BLUETOOTH_CLIENT_GET_PRIVATE(client);
	const char *property = g_param_spec_get_name (pspec);
	GtkTreeIter iter;
	gboolean notify = TRUE;
	gboolean is_default;

	if (get_iter_from_proxy (priv->store, &iter, G_DBUS_PROXY (adapter)) == FALSE)
		return;

	gtk_tree_model_get (GTK_TREE_MODEL(priv->store), &iter,
			    BLUETOOTH_COLUMN_DEFAULT, &is_default, -1);

	if (g_strcmp0 (property, "name") == 0) {
		const gchar *name = adapter1_get_name (adapter);

		gtk_tree_store_set (priv->store, &iter,
				    BLUETOOTH_COLUMN_NAME, name, -1);

		if (is_default) {
			g_object_notify (G_OBJECT (client), "default-adapter-powered");
			g_object_notify (G_OBJECT (client), "default-adapter-name");
		}
	} else if (g_strcmp0 (property, "discovering") == 0) {
		gboolean discovering = adapter1_get_discovering (adapter);

		gtk_tree_store_set (priv->store, &iter,
				    BLUETOOTH_COLUMN_DISCOVERING, discovering, -1);

		if (is_default)
			g_object_notify (G_OBJECT (client), "default-adapter-discovering");
	} else if (g_strcmp0 (property, "powered") == 0) {
		gboolean powered = adapter1_get_powered (adapter);

		gtk_tree_store_set (priv->store, &iter,
				    BLUETOOTH_COLUMN_POWERED, powered, -1);

		if (is_default && powered) {
			g_object_notify (G_OBJECT (client), "default-adapter");
			g_object_notify (G_OBJECT (client), "default-adapter-discoverable");
			g_object_notify (G_OBJECT (client), "default-adapter-discovering");
			g_object_notify (G_OBJECT (client), "default-adapter-name");
		}
		g_object_notify (G_OBJECT (client), "default-adapter-powered");
	} else if (g_strcmp0 (property, "discoverable") == 0) {
		gboolean discoverable = adapter1_get_discoverable (adapter);

		gtk_tree_store_set (priv->store, &iter,
				    BLUETOOTH_COLUMN_DISCOVERABLE, discoverable, -1);

		if (is_default)
			g_object_notify (G_OBJECT (client), "default-adapter-discoverable");
	} else {
		notify = FALSE;
	}

	if (notify != FALSE) {
		GtkTreePath *path;

		/* Tell the world */
		path = gtk_tree_model_get_path (GTK_TREE_MODEL (priv->store), &iter);
		gtk_tree_model_row_changed (GTK_TREE_MODEL (priv->store), path, &iter);
		gtk_tree_path_free (path);
	}
}