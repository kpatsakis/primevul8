bluetooth_client_dump_device (GtkTreeModel *model,
			      GtkTreeIter *iter)
{
	GDBusProxy *proxy;
	char *address, *alias, *name, *icon, **uuids;
	gboolean is_default, paired, trusted, connected, discoverable, discovering, powered, is_adapter;
	GtkTreeIter parent;
	BluetoothType type;

	gtk_tree_model_get (model, iter,
			    BLUETOOTH_COLUMN_ADDRESS, &address,
			    BLUETOOTH_COLUMN_ALIAS, &alias,
			    BLUETOOTH_COLUMN_NAME, &name,
			    BLUETOOTH_COLUMN_TYPE, &type,
			    BLUETOOTH_COLUMN_ICON, &icon,
			    BLUETOOTH_COLUMN_DEFAULT, &is_default,
			    BLUETOOTH_COLUMN_PAIRED, &paired,
			    BLUETOOTH_COLUMN_TRUSTED, &trusted,
			    BLUETOOTH_COLUMN_CONNECTED, &connected,
			    BLUETOOTH_COLUMN_DISCOVERABLE, &discoverable,
			    BLUETOOTH_COLUMN_DISCOVERING, &discovering,
			    BLUETOOTH_COLUMN_POWERED, &powered,
			    BLUETOOTH_COLUMN_UUIDS, &uuids,
			    BLUETOOTH_COLUMN_PROXY, &proxy,
			    -1);
	if (proxy) {
		char *basename;
		basename = g_path_get_basename(g_dbus_proxy_get_object_path(proxy));
		is_adapter = !g_str_has_prefix (basename, "dev_");
		g_free (basename);
	} else {
		is_adapter = !gtk_tree_model_iter_parent (model, &parent, iter);
	}

	if (is_adapter != FALSE) {
		/* Adapter */
		g_print ("Adapter: %s (%s)\n", name, address);
		if (is_default)
			g_print ("\tDefault adapter\n");
		g_print ("\tD-Bus Path: %s\n", proxy ? g_dbus_proxy_get_object_path (proxy) : "(none)");
		g_print ("\tDiscoverable: %s\n", BOOL_STR (discoverable));
		if (discovering)
			g_print ("\tDiscovery in progress\n");
		g_print ("\t%s\n", powered ? "Is powered" : "Is not powered");
	} else {
		/* Device */
		g_print ("Device: %s (%s)\n", alias, address);
		g_print ("\tD-Bus Path: %s\n", proxy ? g_dbus_proxy_get_object_path (proxy) : "(none)");
		g_print ("\tType: %s Icon: %s\n", bluetooth_type_to_string (type), icon);
		g_print ("\tPaired: %s Trusted: %s Connected: %s\n", BOOL_STR(paired), BOOL_STR(trusted), BOOL_STR(connected));
		if (uuids != NULL) {
			guint i;
			g_print ("\tUUIDs: ");
			for (i = 0; uuids[i] != NULL; i++)
				g_print ("%s ", uuids[i]);
			g_print ("\n");
		}
	}
	g_print ("\n");

	g_free (alias);
	g_free (address);
	g_free (icon);
	g_clear_object (&proxy);
	g_strfreev (uuids);
}