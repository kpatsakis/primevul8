static int bnxt_re_build_atomic_wqe(const struct ib_send_wr *wr,
				    struct bnxt_qplib_swqe *wqe)
{
	switch (wr->opcode) {
	case IB_WR_ATOMIC_CMP_AND_SWP:
		wqe->type = BNXT_QPLIB_SWQE_TYPE_ATOMIC_CMP_AND_SWP;
		wqe->atomic.cmp_data = atomic_wr(wr)->compare_add;
		wqe->atomic.swap_data = atomic_wr(wr)->swap;
		break;
	case IB_WR_ATOMIC_FETCH_AND_ADD:
		wqe->type = BNXT_QPLIB_SWQE_TYPE_ATOMIC_FETCH_AND_ADD;
		wqe->atomic.cmp_data = atomic_wr(wr)->compare_add;
		break;
	default:
		return -EINVAL;
	}
	wqe->atomic.remote_va = atomic_wr(wr)->remote_addr;
	wqe->atomic.r_key = atomic_wr(wr)->rkey;
	if (wr->send_flags & IB_SEND_SIGNALED)
		wqe->flags |= BNXT_QPLIB_SWQE_FLAGS_SIGNAL_COMP;
	if (wr->send_flags & IB_SEND_FENCE)
		wqe->flags |= BNXT_QPLIB_SWQE_FLAGS_UC_FENCE;
	if (wr->send_flags & IB_SEND_SOLICITED)
		wqe->flags |= BNXT_QPLIB_SWQE_FLAGS_SOLICIT_EVENT;
	return 0;
}