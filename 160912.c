icon_override (const char    *bdaddr,
	       BluetoothType  type)
{
	/* audio-card, you're ugly */
	switch (type) {
	case BLUETOOTH_TYPE_HEADSET:
		return "audio-headset";
	case BLUETOOTH_TYPE_HEADPHONES:
		return "audio-headphones";
	case BLUETOOTH_TYPE_OTHER_AUDIO:
		return "audio-speakers";
	case BLUETOOTH_TYPE_PHONE:
		return phone_oui_to_icon_name (bdaddr);
	case BLUETOOTH_TYPE_DISPLAY:
		return "video-display";
	case BLUETOOTH_TYPE_SCANNER:
		return "scanner";
	case BLUETOOTH_TYPE_REMOTE_CONTROL:
	case BLUETOOTH_TYPE_WEARABLE:
	case BLUETOOTH_TYPE_TOY:
		/* FIXME */
	default:
		return NULL;
	}
}