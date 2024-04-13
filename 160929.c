bluetooth_client_set_trusted (BluetoothClient *client,
			      const char      *device_path,
			      gboolean         trusted)
{
	BluetoothClientPrivate *priv = BLUETOOTH_CLIENT_GET_PRIVATE(client);
	GObject *device;
	GtkTreeIter iter;

	g_return_val_if_fail (BLUETOOTH_IS_CLIENT (client), FALSE);
	g_return_val_if_fail (device != NULL, FALSE);

	if (get_iter_from_path (priv->store, &iter, device_path) == FALSE) {
		g_debug ("Couldn't find device '%s' in tree to mark it as trusted", device_path);
		return FALSE;
	}

	gtk_tree_model_get (GTK_TREE_MODEL (priv->store), &iter,
			    BLUETOOTH_COLUMN_PROXY, &device, -1);

	if (device == NULL)
		return FALSE;

	g_object_set (device, "trusted", trusted, NULL);
	g_object_unref (device);

	return TRUE;
}