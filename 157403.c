static void bnxt_re_process_res_ud_wc(struct bnxt_re_qp *qp,
				      struct ib_wc *wc,
				      struct bnxt_qplib_cqe *cqe)
{
	u8 nw_type;

	wc->opcode = IB_WC_RECV;
	wc->status = __rc_to_ib_wc_status(cqe->status);

	if (cqe->flags & CQ_RES_UD_FLAGS_IMM)
		wc->wc_flags |= IB_WC_WITH_IMM;
	/* report only on GSI QP for Thor */
	if (qp->qplib_qp.type == CMDQ_CREATE_QP_TYPE_GSI) {
		wc->wc_flags |= IB_WC_GRH;
		memcpy(wc->smac, cqe->smac, ETH_ALEN);
		wc->wc_flags |= IB_WC_WITH_SMAC;
		if (cqe->flags & CQ_RES_UD_FLAGS_META_FORMAT_VLAN) {
			wc->vlan_id = (cqe->cfa_meta & 0xFFF);
			if (wc->vlan_id < 0x1000)
				wc->wc_flags |= IB_WC_WITH_VLAN;
		}
		nw_type = (cqe->flags & CQ_RES_UD_FLAGS_ROCE_IP_VER_MASK) >>
			   CQ_RES_UD_FLAGS_ROCE_IP_VER_SFT;
		wc->network_hdr_type = bnxt_re_to_ib_nw_type(nw_type);
		wc->wc_flags |= IB_WC_WITH_NETWORK_HDR_TYPE;
	}

}