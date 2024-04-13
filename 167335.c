static int remove_net_device(struct hso_device *hso_dev)
{
	int i;

	for (i = 0; i < HSO_MAX_NET_DEVICES; i++) {
		if (network_table[i] == hso_dev) {
			network_table[i] = NULL;
			break;
		}
	}
	if (i == HSO_MAX_NET_DEVICES)
		return -1;
	return 0;
}