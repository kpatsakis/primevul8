_bluetooth_client_get_default_adapter_name (BluetoothClient *self)
{
	BluetoothClientPrivate *priv = BLUETOOTH_CLIENT_GET_PRIVATE (self);
	GtkTreePath *path;
	GtkTreeIter iter;
	char *ret;

	if (priv->default_adapter == NULL)
		return NULL;

	path = gtk_tree_row_reference_get_path (priv->default_adapter);
	gtk_tree_model_get_iter (GTK_TREE_MODEL (priv->store), &iter, path);
	gtk_tree_model_get (GTK_TREE_MODEL (priv->store), &iter, BLUETOOTH_COLUMN_NAME, &ret, -1);
	gtk_tree_path_free (path);

	return ret;
}