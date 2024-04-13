_bluetooth_client_get_default_adapter_discovering (BluetoothClient *self)
{
	BluetoothClientPrivate *priv = BLUETOOTH_CLIENT_GET_PRIVATE (self);
	GtkTreePath *path;
	GtkTreeIter iter;
	gboolean ret;

	if (priv->default_adapter == NULL)
		return FALSE;

	path = gtk_tree_row_reference_get_path (priv->default_adapter);
	gtk_tree_model_get_iter (GTK_TREE_MODEL (priv->store), &iter, path);
	gtk_tree_model_get (GTK_TREE_MODEL (priv->store), &iter, BLUETOOTH_COLUMN_DISCOVERING, &ret, -1);
	gtk_tree_path_free (path);

	return ret;
}