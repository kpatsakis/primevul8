static u8 __from_ib_qp_state(enum ib_qp_state state)
{
	switch (state) {
	case IB_QPS_RESET:
		return CMDQ_MODIFY_QP_NEW_STATE_RESET;
	case IB_QPS_INIT:
		return CMDQ_MODIFY_QP_NEW_STATE_INIT;
	case IB_QPS_RTR:
		return CMDQ_MODIFY_QP_NEW_STATE_RTR;
	case IB_QPS_RTS:
		return CMDQ_MODIFY_QP_NEW_STATE_RTS;
	case IB_QPS_SQD:
		return CMDQ_MODIFY_QP_NEW_STATE_SQD;
	case IB_QPS_SQE:
		return CMDQ_MODIFY_QP_NEW_STATE_SQE;
	case IB_QPS_ERR:
	default:
		return CMDQ_MODIFY_QP_NEW_STATE_ERR;
	}
}