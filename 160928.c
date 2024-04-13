static void bluetooth_client_init(BluetoothClient *client)
{
	BluetoothClientPrivate *priv = BLUETOOTH_CLIENT_GET_PRIVATE(client);

	priv->cancellable = g_cancellable_new ();
	priv->store = gtk_tree_store_new(_BLUETOOTH_NUM_COLUMNS,
					 G_TYPE_OBJECT,     /* BLUETOOTH_COLUMN_PROXY */
					 G_TYPE_OBJECT,     /* BLUETOOTH_COLUMN_PROPERTIES */
					 G_TYPE_STRING,     /* BLUETOOTH_COLUMN_ADDRESS */
					 G_TYPE_STRING,     /* BLUETOOTH_COLUMN_ALIAS */
					 G_TYPE_STRING,     /* BLUETOOTH_COLUMN_NAME */
					 G_TYPE_UINT,       /* BLUETOOTH_COLUMN_TYPE */
					 G_TYPE_STRING,     /* BLUETOOTH_COLUMN_ICON */
					 G_TYPE_BOOLEAN,    /* BLUETOOTH_COLUMN_DEFAULT */
					 G_TYPE_BOOLEAN,    /* BLUETOOTH_COLUMN_PAIRED */
					 G_TYPE_BOOLEAN,    /* BLUETOOTH_COLUMN_TRUSTED */
					 G_TYPE_BOOLEAN,    /* BLUETOOTH_COLUMN_CONNECTED */
					 G_TYPE_BOOLEAN,    /* BLUETOOTH_COLUMN_DISCOVERABLE */
					 G_TYPE_BOOLEAN,    /* BLUETOOTH_COLUMN_DISCOVERING */
					 G_TYPE_INT,        /* BLUETOOTH_COLUMN_LEGACYPAIRING */
					 G_TYPE_BOOLEAN,    /* BLUETOOTH_COLUMN_POWERED */
					 G_TYPE_HASH_TABLE, /* BLUETOOTH_COLUMN_SERVICES */
					 G_TYPE_STRV);      /* BLUETOOTH_COLUMN_UUIDS */

	g_dbus_object_manager_client_new_for_bus (G_BUS_TYPE_SYSTEM,
						  G_DBUS_OBJECT_MANAGER_CLIENT_FLAGS_DO_NOT_AUTO_START,
						  BLUEZ_SERVICE,
						  BLUEZ_MANAGER_PATH,
						  object_manager_get_proxy_type_func,
						  NULL, NULL,
						  priv->cancellable,
						  object_manager_new_callback, client);
}