static void bnxt_re_process_res_shadow_qp_wc(struct bnxt_re_qp *qp,
					     struct ib_wc *wc,
					     struct bnxt_qplib_cqe *cqe)
{
	struct bnxt_re_dev *rdev = qp->rdev;
	struct bnxt_re_qp *qp1_qp = NULL;
	struct bnxt_qplib_cqe *orig_cqe = NULL;
	struct bnxt_re_sqp_entries *sqp_entry = NULL;
	int nw_type;
	u32 tbl_idx;
	u16 vlan_id;
	u8 sl;

	tbl_idx = cqe->wr_id;

	sqp_entry = &rdev->sqp_tbl[tbl_idx];
	qp1_qp = sqp_entry->qp1_qp;
	orig_cqe = &sqp_entry->cqe;

	wc->wr_id = sqp_entry->wrid;
	wc->byte_len = orig_cqe->length;
	wc->qp = &qp1_qp->ib_qp;

	wc->ex.imm_data = orig_cqe->immdata;
	wc->src_qp = orig_cqe->src_qp;
	memcpy(wc->smac, orig_cqe->smac, ETH_ALEN);
	if (bnxt_re_is_vlan_pkt(orig_cqe, &vlan_id, &sl)) {
		wc->vlan_id = vlan_id;
		wc->sl = sl;
		wc->wc_flags |= IB_WC_WITH_VLAN;
	}
	wc->port_num = 1;
	wc->vendor_err = orig_cqe->status;

	wc->opcode = IB_WC_RECV;
	wc->status = __rawqp1_to_ib_wc_status(orig_cqe->status);
	wc->wc_flags |= IB_WC_GRH;

	nw_type = bnxt_re_check_packet_type(orig_cqe->raweth_qp1_flags,
					    orig_cqe->raweth_qp1_flags2);
	if (nw_type >= 0) {
		wc->network_hdr_type = bnxt_re_to_ib_nw_type(nw_type);
		wc->wc_flags |= IB_WC_WITH_NETWORK_HDR_TYPE;
	}
}