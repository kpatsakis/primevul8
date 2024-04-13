adapter_added (GDBusObjectManager   *manager,
	       Adapter1             *adapter,
	       BluetoothClient      *client)
{
	BluetoothClientPrivate *priv = BLUETOOTH_CLIENT_GET_PRIVATE(client);
	GtkTreeIter iter;
	const gchar *address, *name;
	gboolean discovering, discoverable, powered;

	g_signal_connect (G_OBJECT (adapter), "notify",
			  G_CALLBACK (adapter_notify_cb), client);

	address = adapter1_get_address (adapter);
	name = adapter1_get_name (adapter);
	discovering = adapter1_get_discovering (adapter);
	powered = adapter1_get_powered (adapter);
	discoverable = adapter1_get_discoverable (adapter);

	gtk_tree_store_insert_with_values(priv->store, &iter, NULL, -1,
					  BLUETOOTH_COLUMN_PROXY, adapter,
					  BLUETOOTH_COLUMN_ADDRESS, address,
					  BLUETOOTH_COLUMN_NAME, name,
					  BLUETOOTH_COLUMN_DISCOVERING, discovering,
					  BLUETOOTH_COLUMN_DISCOVERABLE, discoverable,
					  BLUETOOTH_COLUMN_POWERED, powered,
					  -1);

	if (!priv->default_adapter) {
		default_adapter_changed (manager,
					 g_dbus_object_get_object_path (g_dbus_interface_get_object (G_DBUS_INTERFACE (adapter))),
					 client);
	}
}