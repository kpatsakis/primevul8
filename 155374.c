static void vhost_net_enable_zcopy(int vq)
{
	vhost_net_zcopy_mask |= 0x1 << vq;
}