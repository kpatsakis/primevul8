bluetooth_client_get_property (GObject        *object,
			       guint           property_id,
			       GValue         *value,
			       GParamSpec     *pspec)
{
	BluetoothClient *self = BLUETOOTH_CLIENT (object);

	switch (property_id) {
	case PROP_DEFAULT_ADAPTER:
		g_value_set_string (value, _bluetooth_client_get_default_adapter_path (self));
		break;
	case PROP_DEFAULT_ADAPTER_POWERED:
		g_value_set_boolean (value, _bluetooth_client_get_default_adapter_powered (self));
		break;
	case PROP_DEFAULT_ADAPTER_NAME:
		g_value_take_string (value, _bluetooth_client_get_default_adapter_name (self));
		break;
	case PROP_DEFAULT_ADAPTER_DISCOVERABLE:
		g_value_set_boolean (value, _bluetooth_client_get_discoverable (self));
		break;
	case PROP_DEFAULT_ADAPTER_DISCOVERING:
		g_value_set_boolean (value, _bluetooth_client_get_default_adapter_discovering (self));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
		break;
	}
}