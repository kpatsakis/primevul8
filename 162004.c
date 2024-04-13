int __init udpv4_offload_init(void)
{
	return inet_add_offload(&udpv4_offload, IPPROTO_UDP);
}