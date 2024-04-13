static enum ib_qp_state __to_ib_qp_state(u8 state)
{
	switch (state) {
	case CMDQ_MODIFY_QP_NEW_STATE_RESET:
		return IB_QPS_RESET;
	case CMDQ_MODIFY_QP_NEW_STATE_INIT:
		return IB_QPS_INIT;
	case CMDQ_MODIFY_QP_NEW_STATE_RTR:
		return IB_QPS_RTR;
	case CMDQ_MODIFY_QP_NEW_STATE_RTS:
		return IB_QPS_RTS;
	case CMDQ_MODIFY_QP_NEW_STATE_SQD:
		return IB_QPS_SQD;
	case CMDQ_MODIFY_QP_NEW_STATE_SQE:
		return IB_QPS_SQE;
	case CMDQ_MODIFY_QP_NEW_STATE_ERR:
	default:
		return IB_QPS_ERR;
	}
}