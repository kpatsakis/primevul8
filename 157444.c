int bnxt_re_poll_cq(struct ib_cq *ib_cq, int num_entries, struct ib_wc *wc)
{
	struct bnxt_re_cq *cq = container_of(ib_cq, struct bnxt_re_cq, ib_cq);
	struct bnxt_re_qp *qp;
	struct bnxt_qplib_cqe *cqe;
	int i, ncqe, budget;
	struct bnxt_qplib_q *sq;
	struct bnxt_qplib_qp *lib_qp;
	u32 tbl_idx;
	struct bnxt_re_sqp_entries *sqp_entry = NULL;
	unsigned long flags;

	spin_lock_irqsave(&cq->cq_lock, flags);
	budget = min_t(u32, num_entries, cq->max_cql);
	num_entries = budget;
	if (!cq->cql) {
		dev_err(rdev_to_dev(cq->rdev), "POLL CQ : no CQL to use");
		goto exit;
	}
	cqe = &cq->cql[0];
	while (budget) {
		lib_qp = NULL;
		ncqe = bnxt_qplib_poll_cq(&cq->qplib_cq, cqe, budget, &lib_qp);
		if (lib_qp) {
			sq = &lib_qp->sq;
			if (sq->send_phantom) {
				qp = container_of(lib_qp,
						  struct bnxt_re_qp, qplib_qp);
				if (send_phantom_wqe(qp) == -ENOMEM)
					dev_err(rdev_to_dev(cq->rdev),
						"Phantom failed! Scheduled to send again\n");
				else
					sq->send_phantom = false;
			}
		}
		if (ncqe < budget)
			ncqe += bnxt_qplib_process_flush_list(&cq->qplib_cq,
							      cqe + ncqe,
							      budget - ncqe);

		if (!ncqe)
			break;

		for (i = 0; i < ncqe; i++, cqe++) {
			/* Transcribe each qplib_wqe back to ib_wc */
			memset(wc, 0, sizeof(*wc));

			wc->wr_id = cqe->wr_id;
			wc->byte_len = cqe->length;
			qp = container_of
				((struct bnxt_qplib_qp *)
				 (unsigned long)(cqe->qp_handle),
				 struct bnxt_re_qp, qplib_qp);
			if (!qp) {
				dev_err(rdev_to_dev(cq->rdev),
					"POLL CQ : bad QP handle");
				continue;
			}
			wc->qp = &qp->ib_qp;
			wc->ex.imm_data = cqe->immdata;
			wc->src_qp = cqe->src_qp;
			memcpy(wc->smac, cqe->smac, ETH_ALEN);
			wc->port_num = 1;
			wc->vendor_err = cqe->status;

			switch (cqe->opcode) {
			case CQ_BASE_CQE_TYPE_REQ:
				if (qp->rdev->qp1_sqp && qp->qplib_qp.id ==
				    qp->rdev->qp1_sqp->qplib_qp.id) {
					/* Handle this completion with
					 * the stored completion
					 */
					memset(wc, 0, sizeof(*wc));
					continue;
				}
				bnxt_re_process_req_wc(wc, cqe);
				break;
			case CQ_BASE_CQE_TYPE_RES_RAWETH_QP1:
				if (!cqe->status) {
					int rc = 0;

					rc = bnxt_re_process_raw_qp_pkt_rx
								(qp, cqe);
					if (!rc) {
						memset(wc, 0, sizeof(*wc));
						continue;
					}
					cqe->status = -1;
				}
				/* Errors need not be looped back.
				 * But change the wr_id to the one
				 * stored in the table
				 */
				tbl_idx = cqe->wr_id;
				sqp_entry = &cq->rdev->sqp_tbl[tbl_idx];
				wc->wr_id = sqp_entry->wrid;
				bnxt_re_process_res_rawqp1_wc(wc, cqe);
				break;
			case CQ_BASE_CQE_TYPE_RES_RC:
				bnxt_re_process_res_rc_wc(wc, cqe);
				break;
			case CQ_BASE_CQE_TYPE_RES_UD:
				if (qp->rdev->qp1_sqp && qp->qplib_qp.id ==
				    qp->rdev->qp1_sqp->qplib_qp.id) {
					/* Handle this completion with
					 * the stored completion
					 */
					if (cqe->status) {
						continue;
					} else {
						bnxt_re_process_res_shadow_qp_wc
								(qp, wc, cqe);
						break;
					}
				}
				bnxt_re_process_res_ud_wc(qp, wc, cqe);
				break;
			default:
				dev_err(rdev_to_dev(cq->rdev),
					"POLL CQ : type 0x%x not handled",
					cqe->opcode);
				continue;
			}
			wc++;
			budget--;
		}
	}
exit:
	spin_unlock_irqrestore(&cq->cq_lock, flags);
	return num_entries - budget;
}