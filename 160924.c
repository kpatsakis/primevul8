_bluetooth_client_get_discoverable (BluetoothClient *client)
{
	BluetoothClientPrivate *priv;
	GtkTreePath *path;
	GtkTreeIter iter;
	gboolean ret;

	g_return_val_if_fail (BLUETOOTH_IS_CLIENT (client), FALSE);

	priv = BLUETOOTH_CLIENT_GET_PRIVATE (client);
	if (priv->default_adapter == NULL)
		return FALSE;

	path = gtk_tree_row_reference_get_path (priv->default_adapter);
	gtk_tree_model_get_iter (GTK_TREE_MODEL (priv->store), &iter, path);
	gtk_tree_model_get (GTK_TREE_MODEL (priv->store), &iter,
                            BLUETOOTH_COLUMN_DISCOVERABLE, &ret, -1);

	return ret;
}