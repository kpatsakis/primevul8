bluetooth_client_setup_device (BluetoothClient          *client,
			       const char               *path,
			       gboolean                  pair,
			       GCancellable             *cancellable,
			       GAsyncReadyCallback       callback,
			       gpointer                  user_data)
{
	BluetoothClientPrivate *priv = BLUETOOTH_CLIENT_GET_PRIVATE(client);
	GSimpleAsyncResult *simple;
	GDBusProxy *device;
	GtkTreeIter iter, adapter_iter;
	gboolean paired;
	GError *err = NULL;

	g_return_if_fail (BLUETOOTH_IS_CLIENT (client));

	simple = g_simple_async_result_new (G_OBJECT (client),
					    callback,
					    user_data,
					    bluetooth_client_setup_device);
	g_simple_async_result_set_check_cancellable (simple, cancellable);
	g_object_set_data (G_OBJECT (simple), "device-object-path", g_strdup (path));

	if (get_iter_from_path (priv->store, &iter, path) == FALSE) {
		g_simple_async_result_set_error (simple, G_IO_ERROR, G_IO_ERROR_INVALID_ARGUMENT,
						 "Device with object path %s does not exist",
						 path);
		g_simple_async_result_complete_in_idle (simple);
		g_object_unref (simple);
		return;
	}

	gtk_tree_model_get (GTK_TREE_MODEL(priv->store), &iter,
			    BLUETOOTH_COLUMN_PROXY, &device,
			    BLUETOOTH_COLUMN_PAIRED, &paired, -1);

	if (paired != FALSE &&
	    gtk_tree_model_iter_parent (GTK_TREE_MODEL(priv->store), &adapter_iter, &iter)) {
		GDBusProxy *adapter;

		gtk_tree_model_get (GTK_TREE_MODEL(priv->store), &adapter_iter,
				    BLUETOOTH_COLUMN_PROXY, &adapter,
				    -1);
		adapter1_call_remove_device_sync (ADAPTER1 (adapter),
						  path,
						  NULL, &err);
		if (err != NULL) {
			g_warning ("Failed to remove device: %s", err->message);
			g_error_free (err);
		}
		g_object_unref (adapter);
	}

	if (pair == TRUE) {
		device1_call_pair (DEVICE1(device),
				   cancellable,
				   (GAsyncReadyCallback) device_pair_callback,
				   simple);
	} else {
		g_simple_async_result_set_op_res_gboolean (simple, TRUE);
		g_simple_async_result_complete_in_idle (simple);
		g_object_unref (simple);
	}

	g_object_unref (device);
}