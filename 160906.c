_bluetooth_client_set_default_adapter_discovering (BluetoothClient *client,
						   gboolean         discover)
{
	BluetoothClientPrivate *priv = BLUETOOTH_CLIENT_GET_PRIVATE (client);
	GtkTreeIter iter;
	GDBusProxy *adapter;
	gboolean current;

	adapter = _bluetooth_client_get_default_adapter (client);
	if (adapter == NULL)
		return;

	get_iter_from_proxy (priv->store, &iter, adapter);
	gtk_tree_model_get (GTK_TREE_MODEL (priv->store), &iter,
			    BLUETOOTH_COLUMN_DISCOVERING, &current, -1);

	if (current == discover) {
		g_object_unref(adapter);
		return;
	}

	if (discover)
		adapter1_call_start_discovery_sync (ADAPTER1 (adapter), NULL, NULL);
	else
		adapter1_call_stop_discovery_sync (ADAPTER1 (adapter), NULL, NULL);

	g_object_unref(adapter);
}