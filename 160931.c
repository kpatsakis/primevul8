GtkTreeModel *bluetooth_client_get_adapter_model (BluetoothClient *client)
{
	return bluetooth_client_get_filter_model (client, adapter_filter,
						  NULL, NULL);
}