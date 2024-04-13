enum rdma_link_layer bnxt_re_get_link_layer(struct ib_device *ibdev,
					    u8 port_num)
{
	return IB_LINK_LAYER_ETHERNET;
}