BluetoothClient *bluetooth_client_new(void)
{
	static BluetoothClient *bluetooth_client = NULL;

	if (bluetooth_client != NULL)
		return g_object_ref(bluetooth_client);

	bluetooth_client = BLUETOOTH_CLIENT (g_object_new (BLUETOOTH_TYPE_CLIENT, NULL));
	g_object_add_weak_pointer (G_OBJECT (bluetooth_client),
				   (gpointer) &bluetooth_client);

	return bluetooth_client;
}