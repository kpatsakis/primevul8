static int bnxt_re_to_ib_nw_type(int nw_type)
{
	u8 nw_hdr_type = 0xFF;

	switch (nw_type) {
	case BNXT_RE_ROCE_V1_PACKET:
		nw_hdr_type = RDMA_NETWORK_ROCE_V1;
		break;
	case BNXT_RE_ROCEV2_IPV4_PACKET:
		nw_hdr_type = RDMA_NETWORK_IPV4;
		break;
	case BNXT_RE_ROCEV2_IPV6_PACKET:
		nw_hdr_type = RDMA_NETWORK_IPV6;
		break;
	}
	return nw_hdr_type;
}