device_notify_cb (Device1         *device,
		  GParamSpec      *pspec,
		  BluetoothClient *client)
{
	BluetoothClientPrivate *priv = BLUETOOTH_CLIENT_GET_PRIVATE(client);
	const char *property = g_param_spec_get_name (pspec);
	GtkTreeIter iter;

	if (get_iter_from_proxy (priv->store, &iter, G_DBUS_PROXY (device)) == FALSE)
		return;

	if (g_strcmp0 (property, "name") == 0) {
		const gchar *name = device1_get_name (device);

		gtk_tree_store_set (priv->store, &iter,
				    BLUETOOTH_COLUMN_NAME, name, -1);
	} else if (g_strcmp0 (property, "alias") == 0) {
		const gchar *alias = device1_get_alias (device);

		gtk_tree_store_set (priv->store, &iter,
				    BLUETOOTH_COLUMN_ALIAS, alias, -1);
	} else if (g_strcmp0 (property, "paired") == 0) {
		gboolean paired = device1_get_paired (device);

		gtk_tree_store_set (priv->store, &iter,
				    BLUETOOTH_COLUMN_PAIRED, paired, -1);
	} else if (g_strcmp0 (property, "trusted") == 0) {
		gboolean trusted = device1_get_trusted (device);

		gtk_tree_store_set (priv->store, &iter,
				    BLUETOOTH_COLUMN_TRUSTED, trusted, -1);
	} else if (g_strcmp0 (property, "connected") == 0) {
		gboolean connected = device1_get_connected (device);

		gtk_tree_store_set (priv->store, &iter,
				    BLUETOOTH_COLUMN_CONNECTED, connected, -1);
	} else if (g_strcmp0 (property, "uuids") == 0) {
		char **uuids;

		uuids = device_list_uuids (device1_get_uuids (device));

		gtk_tree_store_set (priv->store, &iter,
				    BLUETOOTH_COLUMN_UUIDS, uuids, -1);
		g_strfreev (uuids);
	} else if (g_strcmp0 (property, "legacy-pairing") == 0) {
		gboolean legacypairing = device1_get_legacy_pairing (device);

		gtk_tree_store_set (priv->store, &iter,
				    BLUETOOTH_COLUMN_LEGACYPAIRING, legacypairing,
				    -1);
	} else if (g_strcmp0 (property, "icon") == 0 ||
		   g_strcmp0 (property, "class") == 0 ||
		   g_strcmp0 (property, "appearance") == 0) {
		BluetoothType type = BLUETOOTH_TYPE_ANY;
		const char *icon = NULL;

		device_resolve_type_and_icon (device, &type, &icon);

		gtk_tree_store_set (priv->store, &iter,
				    BLUETOOTH_COLUMN_TYPE, type,
				    BLUETOOTH_COLUMN_ICON, icon,
				    -1);
	} else {
		g_debug ("Unhandled property: %s", property);
	}
}