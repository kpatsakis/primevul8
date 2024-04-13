device_added (GDBusObjectManager   *manager,
	      Device1              *device,
	      BluetoothClient      *client)
{
	BluetoothClientPrivate *priv = BLUETOOTH_CLIENT_GET_PRIVATE(client);
	GDBusProxy *adapter;
	const char *adapter_path, *address, *alias, *name, *icon;
	char **uuids;
	gboolean paired, trusted, connected;
	int legacypairing;
	BluetoothType type;
	GtkTreeIter iter, parent;

	g_signal_connect (G_OBJECT (device), "notify",
			  G_CALLBACK (device_notify_cb), client);

	adapter_path = device1_get_adapter (device);
	address = device1_get_address (device);
	alias = device1_get_alias (device);
	name = device1_get_name (device);
	paired = device1_get_paired (device);
	trusted = device1_get_trusted (device);
	connected = device1_get_connected (device);
	uuids = device_list_uuids (device1_get_uuids (device));
	legacypairing = device1_get_legacy_pairing (device);

	device_resolve_type_and_icon (device, &type, &icon);

	if (get_iter_from_path (priv->store, &parent, adapter_path) == FALSE)
		return;

	gtk_tree_model_get (GTK_TREE_MODEL(priv->store), &parent,
			    BLUETOOTH_COLUMN_PROXY, &adapter, -1);

	if (get_iter_from_address (priv->store, &iter, address, adapter) == FALSE) {
		gtk_tree_store_insert_with_values (priv->store, &iter, &parent, -1,
						   BLUETOOTH_COLUMN_ADDRESS, address,
						   BLUETOOTH_COLUMN_ALIAS, alias,
						   BLUETOOTH_COLUMN_NAME, name,
						   BLUETOOTH_COLUMN_TYPE, type,
						   BLUETOOTH_COLUMN_ICON, icon,
						   BLUETOOTH_COLUMN_LEGACYPAIRING, legacypairing,
						   BLUETOOTH_COLUMN_UUIDS, uuids,
						   BLUETOOTH_COLUMN_PAIRED, paired,
						   BLUETOOTH_COLUMN_CONNECTED, connected,
						   BLUETOOTH_COLUMN_TRUSTED, trusted,
						   BLUETOOTH_COLUMN_PROXY, device,
						   -1);
	} else {
		gtk_tree_store_set(priv->store, &iter,
				   BLUETOOTH_COLUMN_ADDRESS, address,
				   BLUETOOTH_COLUMN_ALIAS, alias,
				   BLUETOOTH_COLUMN_NAME, name,
				   BLUETOOTH_COLUMN_TYPE, type,
				   BLUETOOTH_COLUMN_ICON, icon,
				   BLUETOOTH_COLUMN_LEGACYPAIRING, legacypairing,
				   BLUETOOTH_COLUMN_UUIDS, uuids,
				   BLUETOOTH_COLUMN_PAIRED, paired,
				   BLUETOOTH_COLUMN_CONNECTED, connected,
				   BLUETOOTH_COLUMN_TRUSTED, trusted,
				   BLUETOOTH_COLUMN_PROXY, device,
				   -1);
	}
	g_strfreev (uuids);
	g_object_unref (adapter);
}