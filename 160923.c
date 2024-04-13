_bluetooth_client_get_default_adapter(BluetoothClient *client)
{
	BluetoothClientPrivate *priv = BLUETOOTH_CLIENT_GET_PRIVATE(client);
	GtkTreePath *path;
	GtkTreeIter iter;
	GDBusProxy *adapter;

	g_return_val_if_fail (BLUETOOTH_IS_CLIENT (client), NULL);

	if (priv->default_adapter == NULL)
		return NULL;

	path = gtk_tree_row_reference_get_path (priv->default_adapter);
	gtk_tree_model_get_iter (GTK_TREE_MODEL (priv->store), &iter, path);
	gtk_tree_model_get (GTK_TREE_MODEL (priv->store), &iter,
			    BLUETOOTH_COLUMN_PROXY, &adapter, -1);
	gtk_tree_path_free (path);

	return adapter;
}