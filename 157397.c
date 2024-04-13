static enum ib_mtu __to_ib_mtu(u32 mtu)
{
	switch (mtu & CREQ_QUERY_QP_RESP_SB_PATH_MTU_MASK) {
	case CMDQ_MODIFY_QP_PATH_MTU_MTU_256:
		return IB_MTU_256;
	case CMDQ_MODIFY_QP_PATH_MTU_MTU_512:
		return IB_MTU_512;
	case CMDQ_MODIFY_QP_PATH_MTU_MTU_1024:
		return IB_MTU_1024;
	case CMDQ_MODIFY_QP_PATH_MTU_MTU_2048:
		return IB_MTU_2048;
	case CMDQ_MODIFY_QP_PATH_MTU_MTU_4096:
		return IB_MTU_4096;
	default:
		return IB_MTU_2048;
	}
}