static int bnxt_re_process_raw_qp_pkt_rx(struct bnxt_re_qp *qp1_qp,
					 struct bnxt_qplib_cqe *cqe)
{
	struct bnxt_re_dev *rdev = qp1_qp->rdev;
	struct bnxt_re_sqp_entries *sqp_entry = NULL;
	struct bnxt_re_qp *qp = rdev->qp1_sqp;
	struct ib_send_wr *swr;
	struct ib_ud_wr udwr;
	struct ib_recv_wr rwr;
	int pkt_type = 0;
	u32 tbl_idx;
	void *rq_hdr_buf;
	dma_addr_t rq_hdr_buf_map;
	dma_addr_t shrq_hdr_buf_map;
	u32 offset = 0;
	u32 skip_bytes = 0;
	struct ib_sge s_sge[2];
	struct ib_sge r_sge[2];
	int rc;

	memset(&udwr, 0, sizeof(udwr));
	memset(&rwr, 0, sizeof(rwr));
	memset(&s_sge, 0, sizeof(s_sge));
	memset(&r_sge, 0, sizeof(r_sge));

	swr = &udwr.wr;
	tbl_idx = cqe->wr_id;

	rq_hdr_buf = qp1_qp->qplib_qp.rq_hdr_buf +
			(tbl_idx * qp1_qp->qplib_qp.rq_hdr_buf_size);
	rq_hdr_buf_map = bnxt_qplib_get_qp_buf_from_index(&qp1_qp->qplib_qp,
							  tbl_idx);

	/* Shadow QP header buffer */
	shrq_hdr_buf_map = bnxt_qplib_get_qp_buf_from_index(&qp->qplib_qp,
							    tbl_idx);
	sqp_entry = &rdev->sqp_tbl[tbl_idx];

	/* Store this cqe */
	memcpy(&sqp_entry->cqe, cqe, sizeof(struct bnxt_qplib_cqe));
	sqp_entry->qp1_qp = qp1_qp;

	/* Find packet type from the cqe */

	pkt_type = bnxt_re_check_packet_type(cqe->raweth_qp1_flags,
					     cqe->raweth_qp1_flags2);
	if (pkt_type < 0) {
		dev_err(rdev_to_dev(rdev), "Invalid packet\n");
		return -EINVAL;
	}

	/* Adjust the offset for the user buffer and post in the rq */

	if (pkt_type == BNXT_RE_ROCEV2_IPV4_PACKET)
		offset = 20;

	/*
	 * QP1 loopback packet has 4 bytes of internal header before
	 * ether header. Skip these four bytes.
	 */
	if (bnxt_re_is_loopback_packet(rdev, rq_hdr_buf))
		skip_bytes = 4;

	/* First send SGE . Skip the ether header*/
	s_sge[0].addr = rq_hdr_buf_map + BNXT_QPLIB_MAX_QP1_RQ_ETH_HDR_SIZE
			+ skip_bytes;
	s_sge[0].lkey = 0xFFFFFFFF;
	s_sge[0].length = offset ? BNXT_QPLIB_MAX_GRH_HDR_SIZE_IPV4 :
				BNXT_QPLIB_MAX_GRH_HDR_SIZE_IPV6;

	/* Second Send SGE */
	s_sge[1].addr = s_sge[0].addr + s_sge[0].length +
			BNXT_QPLIB_MAX_QP1_RQ_BDETH_HDR_SIZE;
	if (pkt_type != BNXT_RE_ROCE_V1_PACKET)
		s_sge[1].addr += 8;
	s_sge[1].lkey = 0xFFFFFFFF;
	s_sge[1].length = 256;

	/* First recv SGE */

	r_sge[0].addr = shrq_hdr_buf_map;
	r_sge[0].lkey = 0xFFFFFFFF;
	r_sge[0].length = 40;

	r_sge[1].addr = sqp_entry->sge.addr + offset;
	r_sge[1].lkey = sqp_entry->sge.lkey;
	r_sge[1].length = BNXT_QPLIB_MAX_GRH_HDR_SIZE_IPV6 + 256 - offset;

	/* Create receive work request */
	rwr.num_sge = 2;
	rwr.sg_list = r_sge;
	rwr.wr_id = tbl_idx;
	rwr.next = NULL;

	rc = bnxt_re_post_recv_shadow_qp(rdev, qp, &rwr);
	if (rc) {
		dev_err(rdev_to_dev(rdev),
			"Failed to post Rx buffers to shadow QP");
		return -ENOMEM;
	}

	swr->num_sge = 2;
	swr->sg_list = s_sge;
	swr->wr_id = tbl_idx;
	swr->opcode = IB_WR_SEND;
	swr->next = NULL;

	udwr.ah = &rdev->sqp_ah->ib_ah;
	udwr.remote_qpn = rdev->qp1_sqp->qplib_qp.id;
	udwr.remote_qkey = rdev->qp1_sqp->qplib_qp.qkey;

	/* post data received  in the send queue */
	rc = bnxt_re_post_send_shadow_qp(rdev, qp, swr);

	return 0;
}