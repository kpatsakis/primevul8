static void bluetooth_client_finalize(GObject *object)
{
	BluetoothClient *client = BLUETOOTH_CLIENT (object);
	BluetoothClientPrivate *priv = BLUETOOTH_CLIENT_GET_PRIVATE (client);

	if (priv->cancellable != NULL) {
		g_cancellable_cancel (priv->cancellable);
		g_clear_object (&priv->cancellable);
	}
	g_clear_object (&priv->manager);
	g_object_unref (priv->store);

	g_clear_pointer (&priv->default_adapter, gtk_tree_row_reference_free);

	G_OBJECT_CLASS(bluetooth_client_parent_class)->finalize (object);
}