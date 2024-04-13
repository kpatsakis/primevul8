static inline struct hso_serial *dev2ser(struct hso_device *hso_dev)
{
	return hso_dev->port_data.dev_serial;
}