GtkTreeModel *bluetooth_client_get_model (BluetoothClient *client)
{
	BluetoothClientPrivate *priv;
	GtkTreeModel *model;

	g_return_val_if_fail (BLUETOOTH_IS_CLIENT (client), NULL);

	priv = BLUETOOTH_CLIENT_GET_PRIVATE(client);
	model = GTK_TREE_MODEL (g_object_ref(priv->store));

	return model;
}