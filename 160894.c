bluetooth_client_connect_service (BluetoothClient     *client,
				  const char          *path,
				  gboolean             connect,
				  GCancellable        *cancellable,
				  GAsyncReadyCallback  callback,
				  gpointer             user_data)
{
	BluetoothClientPrivate *priv = BLUETOOTH_CLIENT_GET_PRIVATE(client);
	GtkTreeIter iter;
	GSimpleAsyncResult *simple;
	GDBusProxy *device;

	g_return_if_fail (BLUETOOTH_IS_CLIENT (client));
	g_return_if_fail (path != NULL);

	if (get_iter_from_path (priv->store, &iter, path) == FALSE)
		return;

	gtk_tree_model_get (GTK_TREE_MODEL(priv->store), &iter,
			    BLUETOOTH_COLUMN_PROXY, &device,
			    -1);

	simple = g_simple_async_result_new (G_OBJECT (client),
					    callback,
					    user_data,
					    bluetooth_client_connect_service);
	g_simple_async_result_set_check_cancellable (simple, cancellable);

	if (connect) {
		device1_call_connect (DEVICE1(device),
				      cancellable,
				      (GAsyncReadyCallback) connect_callback,
				      simple);
	} else {
		device1_call_disconnect (DEVICE1(device),
					 cancellable,
					 (GAsyncReadyCallback) disconnect_callback,
					 simple);
	}

	g_object_unref (device);
}