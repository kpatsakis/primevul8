adapter_removed (GDBusObjectManager   *manager,
		 const char           *path,
		 BluetoothClient      *client)
{
	BluetoothClientPrivate *priv = BLUETOOTH_CLIENT_GET_PRIVATE(client);
	GtkTreeIter iter, childiter;
	gboolean was_default;
	gboolean have_child;

	if (get_iter_from_path (priv->store, &iter, path) == FALSE)
		return;

	gtk_tree_model_get (GTK_TREE_MODEL(priv->store), &iter,
			   BLUETOOTH_COLUMN_DEFAULT, &was_default, -1);

	if (!was_default)
		return;

	/* Ensure that all devices are removed. This can happen if bluetoothd
	 * crashes as the "object-removed" signal is emitted in an undefined
	 * order. */
	have_child = gtk_tree_model_iter_children (priv->store, &childiter, &iter);
	while (have_child) {
		GDBusProxy *object;

		gtk_tree_model_get (GTK_TREE_MODEL(priv->store), &childiter,
				    BLUETOOTH_COLUMN_PROXY, &object, -1);

		g_signal_emit (G_OBJECT (client), signals[DEVICE_REMOVED], 0, g_dbus_proxy_get_object_path (object));
		g_object_unref (object);

		have_child = gtk_tree_store_remove (priv->store, &childiter);
	}

	g_clear_pointer (&priv->default_adapter, gtk_tree_row_reference_free);
	gtk_tree_store_remove (priv->store, &iter);

	if (gtk_tree_model_get_iter_first (GTK_TREE_MODEL(priv->store),
					   &iter)) {
		GDBusProxy *adapter;
		const char *adapter_path;

		gtk_tree_model_get (GTK_TREE_MODEL(priv->store), &iter,
				   BLUETOOTH_COLUMN_PROXY, &adapter, -1);

		adapter_path = g_dbus_proxy_get_object_path (adapter);
		default_adapter_changed (manager, adapter_path, client);

		g_object_unref(adapter);
	} else {
		g_object_notify (G_OBJECT (client), "default-adapter");
		g_object_notify (G_OBJECT (client), "default-adapter-powered");
		g_object_notify (G_OBJECT (client), "default-adapter-discoverable");
		g_object_notify (G_OBJECT (client), "default-adapter-discovering");
	}
}