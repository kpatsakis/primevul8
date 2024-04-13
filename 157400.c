static u8 bnxt_re_stack_to_dev_nw_type(enum rdma_network_type ntype)
{
	u8 nw_type;

	switch (ntype) {
	case RDMA_NETWORK_IPV4:
		nw_type = CMDQ_CREATE_AH_TYPE_V2IPV4;
		break;
	case RDMA_NETWORK_IPV6:
		nw_type = CMDQ_CREATE_AH_TYPE_V2IPV6;
		break;
	default:
		nw_type = CMDQ_CREATE_AH_TYPE_V1;
		break;
	}
	return nw_type;
}