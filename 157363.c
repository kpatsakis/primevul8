static int bnxt_re_build_qp1_shadow_qp_recv(struct bnxt_re_qp *qp,
					    const struct ib_recv_wr *wr,
					    struct bnxt_qplib_swqe *wqe,
					    int payload_size)
{
	struct bnxt_qplib_sge ref, sge;
	u32 rq_prod_index;
	struct bnxt_re_sqp_entries *sqp_entry;

	rq_prod_index = bnxt_qplib_get_rq_prod_index(&qp->qplib_qp);

	if (!bnxt_qplib_get_qp1_rq_buf(&qp->qplib_qp, &sge))
		return -ENOMEM;

	/* Create 1 SGE to receive the entire
	 * ethernet packet
	 */
	/* Save the reference from ULP */
	ref.addr = wqe->sg_list[0].addr;
	ref.lkey = wqe->sg_list[0].lkey;
	ref.size = wqe->sg_list[0].size;

	sqp_entry = &qp->rdev->sqp_tbl[rq_prod_index];

	/* SGE 1 */
	wqe->sg_list[0].addr = sge.addr;
	wqe->sg_list[0].lkey = sge.lkey;
	wqe->sg_list[0].size = BNXT_QPLIB_MAX_QP1_RQ_HDR_SIZE_V2;
	sge.size -= wqe->sg_list[0].size;

	sqp_entry->sge.addr = ref.addr;
	sqp_entry->sge.lkey = ref.lkey;
	sqp_entry->sge.size = ref.size;
	/* Store the wrid for reporting completion */
	sqp_entry->wrid = wqe->wr_id;
	/* change the wqe->wrid to table index */
	wqe->wr_id = rq_prod_index;
	return 0;
}