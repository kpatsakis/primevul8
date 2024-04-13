GtkTreeModel *bluetooth_client_get_device_model (BluetoothClient *client)
{
	BluetoothClientPrivate *priv;
	GtkTreeModel *model;
	GtkTreePath *path;
	GtkTreeIter iter;
	gboolean cont, found = FALSE;

	g_return_val_if_fail (BLUETOOTH_IS_CLIENT (client), NULL);

	priv = BLUETOOTH_CLIENT_GET_PRIVATE(client);
	cont = gtk_tree_model_get_iter_first (GTK_TREE_MODEL(priv->store), &iter);

	while (cont == TRUE) {
		gboolean is_default;

		gtk_tree_model_get (GTK_TREE_MODEL(priv->store), &iter,
				    BLUETOOTH_COLUMN_DEFAULT, &is_default, -1);

		if (is_default == TRUE) {
			found = TRUE;
			break;
		}

		cont = gtk_tree_model_iter_next (GTK_TREE_MODEL(priv->store), &iter);
	}

	if (found == TRUE) {
		path = gtk_tree_model_get_path (GTK_TREE_MODEL(priv->store), &iter);
		model = gtk_tree_model_filter_new (GTK_TREE_MODEL(priv->store), path);
		gtk_tree_path_free (path);
	} else
		model = NULL;

	return model;
}