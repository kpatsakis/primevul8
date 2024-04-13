device_resolve_type_and_icon (Device1 *device, BluetoothType *type, const char **icon)
{
	g_return_if_fail (type);
	g_return_if_fail (icon);

	*type = bluetooth_appearance_to_type (device1_get_appearance (device));
	if (*type == 0 || *type == BLUETOOTH_TYPE_ANY)
		*type = bluetooth_class_to_type (device1_get_class (device));

	*icon = icon_override (device1_get_address (device), *type);

	if (!*icon)
		*icon = device1_get_icon (device);

	if (!*icon)
		*icon = "bluetooth";
}