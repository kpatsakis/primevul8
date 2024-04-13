static u32 hso_mux_to_port(int mux)
{
	u32 result;

	switch (mux) {
	case 0x1:
		result = HSO_PORT_CONTROL;
		break;
	case 0x2:
		result = HSO_PORT_APP;
		break;
	case 0x4:
		result = HSO_PORT_PCSC;
		break;
	case 0x8:
		result = HSO_PORT_GPS;
		break;
	case 0x10:
		result = HSO_PORT_APP2;
		break;
	default:
		result = HSO_PORT_NO_PORT;
	}
	return result;
}