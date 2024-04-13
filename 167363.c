static inline struct hso_net *dev2net(struct hso_device *hso_dev)
{
	return hso_dev->port_data.dev_net;
}