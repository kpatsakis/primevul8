interface_added (GDBusObjectManager *manager,
		 GDBusObject        *object,
		 GDBusInterface     *interface,
		 gpointer            user_data)
{
	BluetoothClient *client = user_data;

	if (IS_ADAPTER1 (interface)) {
		adapter_added (manager,
			       ADAPTER1 (interface),
			       client);
	} else if (IS_DEVICE1 (interface)) {
		device_added (manager,
			      DEVICE1 (interface),
			      client);
	}
}