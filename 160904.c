_bluetooth_client_get_default_adapter_path (BluetoothClient *self)
{
	GDBusProxy *adapter = _bluetooth_client_get_default_adapter (self);

	if (adapter != NULL) {
		const char *ret = g_dbus_proxy_get_object_path (adapter);
		g_object_unref (adapter);
		return ret;
	}
	return NULL;
}