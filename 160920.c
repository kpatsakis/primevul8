bluetooth_client_get_device (BluetoothClient *client,
			     const char       *path)
{
	BluetoothClientPrivate *priv = BLUETOOTH_CLIENT_GET_PRIVATE(client);
	GtkTreeIter iter;
	GDBusProxy *proxy;

	if (get_iter_from_path (priv->store, &iter, path) == FALSE) {
		return NULL;
	}

	gtk_tree_model_get (GTK_TREE_MODEL(priv->store), &iter,
			                BLUETOOTH_COLUMN_PROXY, &proxy,
			                -1);
	return proxy;
}