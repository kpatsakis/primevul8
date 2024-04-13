interface_removed (GDBusObjectManager *manager,
		   GDBusObject        *object,
		   GDBusInterface     *interface,
		   gpointer            user_data)
{
	BluetoothClient *client = user_data;

	if (IS_ADAPTER1 (interface)) {
		adapter_removed (manager,
				 g_dbus_object_get_object_path (object),
				 client);
	} else if (IS_DEVICE1 (interface)) {
		device_removed (g_dbus_object_get_object_path (object),
				client);
	}
}