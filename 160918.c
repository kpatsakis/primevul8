adapter_set_powered (BluetoothClient *client,
		     const char *path,
		     gboolean powered)
{
	BluetoothClientPrivate *priv = BLUETOOTH_CLIENT_GET_PRIVATE(client);
	GObject *adapter;
	GtkTreeIter iter;

	g_return_val_if_fail (BLUETOOTH_IS_CLIENT (client), FALSE);

	if (get_iter_from_path (priv->store, &iter, path) == FALSE)
		return FALSE;

	gtk_tree_model_get (GTK_TREE_MODEL (priv->store), &iter,
			    BLUETOOTH_COLUMN_PROXY, &adapter, -1);

	if (adapter == NULL)
		return FALSE;

	g_object_set (adapter, "powered", powered, NULL);
	g_object_unref (adapter);

	return TRUE;
}