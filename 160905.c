device_removed (const char      *path,
		BluetoothClient *client)
{
	BluetoothClientPrivate *priv = BLUETOOTH_CLIENT_GET_PRIVATE(client);
	GtkTreeIter iter;

	if (get_iter_from_path(priv->store, &iter, path) == TRUE) {
		/* Note that removal can also happen from adapter_removed. */
		g_signal_emit (G_OBJECT (client), signals[DEVICE_REMOVED], 0, path);
		gtk_tree_store_remove(priv->store, &iter);
	}
}