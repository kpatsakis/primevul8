static ssize_t hso_sysfs_show_porttype(struct device *dev,
				       struct device_attribute *attr,
				       char *buf)
{
	struct hso_device *hso_dev = dev_get_drvdata(dev);
	char *port_name;

	if (!hso_dev)
		return 0;

	switch (hso_dev->port_spec & HSO_PORT_MASK) {
	case HSO_PORT_CONTROL:
		port_name = "Control";
		break;
	case HSO_PORT_APP:
		port_name = "Application";
		break;
	case HSO_PORT_APP2:
		port_name = "Application2";
		break;
	case HSO_PORT_GPS:
		port_name = "GPS";
		break;
	case HSO_PORT_GPS_CONTROL:
		port_name = "GPS Control";
		break;
	case HSO_PORT_PCSC:
		port_name = "PCSC";
		break;
	case HSO_PORT_DIAG:
		port_name = "Diagnostic";
		break;
	case HSO_PORT_DIAG2:
		port_name = "Diagnostic2";
		break;
	case HSO_PORT_MODEM:
		port_name = "Modem";
		break;
	case HSO_PORT_NETWORK:
		port_name = "Network";
		break;
	default:
		port_name = "Unknown";
		break;
	}

	return sprintf(buf, "%s\n", port_name);
}