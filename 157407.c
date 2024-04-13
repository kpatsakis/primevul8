static int bnxt_re_build_rdma_wqe(const struct ib_send_wr *wr,
				  struct bnxt_qplib_swqe *wqe)
{
	switch (wr->opcode) {
	case IB_WR_RDMA_WRITE:
		wqe->type = BNXT_QPLIB_SWQE_TYPE_RDMA_WRITE;
		break;
	case IB_WR_RDMA_WRITE_WITH_IMM:
		wqe->type = BNXT_QPLIB_SWQE_TYPE_RDMA_WRITE_WITH_IMM;
		wqe->rdma.imm_data = wr->ex.imm_data;
		break;
	case IB_WR_RDMA_READ:
		wqe->type = BNXT_QPLIB_SWQE_TYPE_RDMA_READ;
		wqe->rdma.inv_key = wr->ex.invalidate_rkey;
		break;
	default:
		return -EINVAL;
	}
	wqe->rdma.remote_va = rdma_wr(wr)->remote_addr;
	wqe->rdma.r_key = rdma_wr(wr)->rkey;
	if (wr->send_flags & IB_SEND_SIGNALED)
		wqe->flags |= BNXT_QPLIB_SWQE_FLAGS_SIGNAL_COMP;
	if (wr->send_flags & IB_SEND_FENCE)
		wqe->flags |= BNXT_QPLIB_SWQE_FLAGS_UC_FENCE;
	if (wr->send_flags & IB_SEND_SOLICITED)
		wqe->flags |= BNXT_QPLIB_SWQE_FLAGS_SOLICIT_EVENT;
	if (wr->send_flags & IB_SEND_INLINE)
		wqe->flags |= BNXT_QPLIB_SWQE_FLAGS_INLINE;

	return 0;
}