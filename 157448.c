static int bnxt_re_build_inv_wqe(const struct ib_send_wr *wr,
				 struct bnxt_qplib_swqe *wqe)
{
	wqe->type = BNXT_QPLIB_SWQE_TYPE_LOCAL_INV;
	wqe->local_inv.inv_l_key = wr->ex.invalidate_rkey;

	/* Need unconditional fence for local invalidate
	 * opcode to work as expected.
	 */
	wqe->flags |= BNXT_QPLIB_SWQE_FLAGS_UC_FENCE;

	if (wr->send_flags & IB_SEND_SIGNALED)
		wqe->flags |= BNXT_QPLIB_SWQE_FLAGS_SIGNAL_COMP;
	if (wr->send_flags & IB_SEND_SOLICITED)
		wqe->flags |= BNXT_QPLIB_SWQE_FLAGS_SOLICIT_EVENT;

	return 0;
}