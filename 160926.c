static gboolean adapter_filter(GtkTreeModel *model,
					GtkTreeIter *iter, gpointer user_data)
{
	GDBusProxy *proxy;
	gboolean active;

	gtk_tree_model_get(model, iter, BLUETOOTH_COLUMN_PROXY, &proxy, -1);

	if (proxy == NULL)
		return FALSE;

	active = g_str_equal(BLUEZ_ADAPTER_INTERFACE,
					g_dbus_proxy_get_interface_name(proxy));

	g_object_unref(proxy);

	return active;
}