GtkTreeModel *bluetooth_client_get_filter_model (BluetoothClient *client,
						 GtkTreeModelFilterVisibleFunc func,
						 gpointer data, GDestroyNotify destroy)
{
	BluetoothClientPrivate *priv;
	GtkTreeModel *model;

	g_return_val_if_fail (BLUETOOTH_IS_CLIENT (client), NULL);

	priv = BLUETOOTH_CLIENT_GET_PRIVATE(client);
	model = gtk_tree_model_filter_new(GTK_TREE_MODEL(priv->store), NULL);

	gtk_tree_model_filter_set_visible_func(GTK_TREE_MODEL_FILTER(model),
							func, data, destroy);

	return model;
}