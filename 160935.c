bluetooth_client_set_property (GObject        *object,
			       guint           property_id,
			       const GValue   *value,
			       GParamSpec     *pspec)
{
	BluetoothClient *self = BLUETOOTH_CLIENT (object);

	switch (property_id) {
	case PROP_DEFAULT_ADAPTER_DISCOVERABLE:
	        _bluetooth_client_set_discoverable (self, g_value_get_boolean (value), 0);
		break;
	case PROP_DEFAULT_ADAPTER_DISCOVERING:
		_bluetooth_client_set_default_adapter_discovering (self, g_value_get_boolean (value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
		break;
	}
}