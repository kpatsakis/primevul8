static int bnxt_re_copy_wr_payload(struct bnxt_re_dev *rdev,
				   const struct ib_send_wr *wr,
				   struct bnxt_qplib_swqe *wqe)
{
	int payload_sz = 0;

	if (wr->send_flags & IB_SEND_INLINE)
		payload_sz = bnxt_re_copy_inline_data(rdev, wr, wqe);
	else
		payload_sz = bnxt_re_build_sgl(wr->sg_list, wqe->sg_list,
					       wqe->num_sge);

	return payload_sz;
}