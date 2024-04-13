object_manager_new_callback(GObject      *source_object,
			    GAsyncResult *res,
			    void         *user_data)
{
	BluetoothClient *client;
	BluetoothClientPrivate *priv;
	GDBusObjectManager *manager;
	GList *object_list, *l;
	GError *error = NULL;

	manager = g_dbus_object_manager_client_new_for_bus_finish (res, &error);
	if (!manager) {
		if (!g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
			g_warning ("Could not create bluez object manager: %s", error->message);
		g_error_free (error);
		return;
	}

	client = BLUETOOTH_CLIENT (user_data);
	priv = BLUETOOTH_CLIENT_GET_PRIVATE(client);
	priv->manager = manager;

	g_signal_connect (G_OBJECT (priv->manager), "interface-added", (GCallback) interface_added, client);
	g_signal_connect (G_OBJECT (priv->manager), "interface-removed", (GCallback) interface_removed, client);

	g_signal_connect (G_OBJECT (priv->manager), "object-added", (GCallback) object_added, client);
	g_signal_connect (G_OBJECT (priv->manager), "object-removed", (GCallback) object_removed, client);

	object_list = g_dbus_object_manager_get_objects (priv->manager);

	/* We need to add the adapters first, otherwise the devices will
	 * be dropped to the floor, as they wouldn't have a parent in
	 * the treestore */
	for (l = object_list; l != NULL; l = l->next) {
		GDBusObject *object = l->data;
		GDBusInterface *iface;

		iface = g_dbus_object_get_interface (object, BLUEZ_ADAPTER_INTERFACE);
		if (!iface)
			continue;

		adapter_added (priv->manager,
			       ADAPTER1 (iface),
			       client);
	}

	for (l = object_list; l != NULL; l = l->next) {
		GDBusObject *object = l->data;
		GDBusInterface *iface;

		iface = g_dbus_object_get_interface (object, BLUEZ_DEVICE_INTERFACE);
		if (!iface)
			continue;

		device_added (priv->manager,
			      DEVICE1 (iface),
			      client);
	}
	g_list_free_full (object_list, g_object_unref);
}