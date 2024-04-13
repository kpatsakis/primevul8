static void hso_log_port(struct hso_device *hso_dev)
{
	char *port_type;
	char port_dev[20];

	switch (hso_dev->port_spec & HSO_PORT_MASK) {
	case HSO_PORT_CONTROL:
		port_type = "Control";
		break;
	case HSO_PORT_APP:
		port_type = "Application";
		break;
	case HSO_PORT_GPS:
		port_type = "GPS";
		break;
	case HSO_PORT_GPS_CONTROL:
		port_type = "GPS control";
		break;
	case HSO_PORT_APP2:
		port_type = "Application2";
		break;
	case HSO_PORT_PCSC:
		port_type = "PCSC";
		break;
	case HSO_PORT_DIAG:
		port_type = "Diagnostic";
		break;
	case HSO_PORT_DIAG2:
		port_type = "Diagnostic2";
		break;
	case HSO_PORT_MODEM:
		port_type = "Modem";
		break;
	case HSO_PORT_NETWORK:
		port_type = "Network";
		break;
	default:
		port_type = "Unknown";
		break;
	}
	if ((hso_dev->port_spec & HSO_PORT_MASK) == HSO_PORT_NETWORK) {
		sprintf(port_dev, "%s", dev2net(hso_dev)->net->name);
	} else
		sprintf(port_dev, "/dev/%s%d", tty_filename,
			dev2ser(hso_dev)->minor);

	dev_dbg(&hso_dev->interface->dev, "HSO: Found %s port %s\n",
		port_type, port_dev);
}