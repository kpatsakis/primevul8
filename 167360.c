static u32 hso_port_to_mux(int port)
{
	u32 result;

	switch (port & HSO_PORT_MASK) {
	case HSO_PORT_CONTROL:
		result = 0x0;
		break;
	case HSO_PORT_APP:
		result = 0x1;
		break;
	case HSO_PORT_PCSC:
		result = 0x2;
		break;
	case HSO_PORT_GPS:
		result = 0x3;
		break;
	case HSO_PORT_APP2:
		result = 0x4;
		break;
	default:
		result = 0x0;
	}
	return result;
}