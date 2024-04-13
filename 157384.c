static void bnxt_re_process_res_rawqp1_wc(struct ib_wc *wc,
					  struct bnxt_qplib_cqe *cqe)
{
	wc->opcode = IB_WC_RECV;
	wc->status = __rawqp1_to_ib_wc_status(cqe->status);
	wc->wc_flags |= IB_WC_GRH;
}