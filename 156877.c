inet_broadcast(uint32_t network, uint32_t netmask)
{
	return 0xffffffff - netmask + network;
}