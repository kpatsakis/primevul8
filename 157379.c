int bnxt_re_post_send(struct ib_qp *ib_qp, const struct ib_send_wr *wr,
		      const struct ib_send_wr **bad_wr)
{
	struct bnxt_re_qp *qp = container_of(ib_qp, struct bnxt_re_qp, ib_qp);
	struct bnxt_qplib_swqe wqe;
	int rc = 0, payload_sz = 0;
	unsigned long flags;

	spin_lock_irqsave(&qp->sq_lock, flags);
	while (wr) {
		/* House keeping */
		memset(&wqe, 0, sizeof(wqe));

		/* Common */
		wqe.num_sge = wr->num_sge;
		if (wr->num_sge > qp->qplib_qp.sq.max_sge) {
			dev_err(rdev_to_dev(qp->rdev),
				"Limit exceeded for Send SGEs");
			rc = -EINVAL;
			goto bad;
		}

		payload_sz = bnxt_re_copy_wr_payload(qp->rdev, wr, &wqe);
		if (payload_sz < 0) {
			rc = -EINVAL;
			goto bad;
		}
		wqe.wr_id = wr->wr_id;

		switch (wr->opcode) {
		case IB_WR_SEND:
		case IB_WR_SEND_WITH_IMM:
			if (qp->qplib_qp.type == CMDQ_CREATE_QP1_TYPE_GSI) {
				rc = bnxt_re_build_qp1_send_v2(qp, wr, &wqe,
							       payload_sz);
				if (rc)
					goto bad;
				wqe.rawqp1.lflags |=
					SQ_SEND_RAWETH_QP1_LFLAGS_ROCE_CRC;
			}
			switch (wr->send_flags) {
			case IB_SEND_IP_CSUM:
				wqe.rawqp1.lflags |=
					SQ_SEND_RAWETH_QP1_LFLAGS_IP_CHKSUM;
				break;
			default:
				break;
			}
			/* fall through */
		case IB_WR_SEND_WITH_INV:
			rc = bnxt_re_build_send_wqe(qp, wr, &wqe);
			break;
		case IB_WR_RDMA_WRITE:
		case IB_WR_RDMA_WRITE_WITH_IMM:
		case IB_WR_RDMA_READ:
			rc = bnxt_re_build_rdma_wqe(wr, &wqe);
			break;
		case IB_WR_ATOMIC_CMP_AND_SWP:
		case IB_WR_ATOMIC_FETCH_AND_ADD:
			rc = bnxt_re_build_atomic_wqe(wr, &wqe);
			break;
		case IB_WR_RDMA_READ_WITH_INV:
			dev_err(rdev_to_dev(qp->rdev),
				"RDMA Read with Invalidate is not supported");
			rc = -EINVAL;
			goto bad;
		case IB_WR_LOCAL_INV:
			rc = bnxt_re_build_inv_wqe(wr, &wqe);
			break;
		case IB_WR_REG_MR:
			rc = bnxt_re_build_reg_wqe(reg_wr(wr), &wqe);
			break;
		default:
			/* Unsupported WRs */
			dev_err(rdev_to_dev(qp->rdev),
				"WR (%#x) is not supported", wr->opcode);
			rc = -EINVAL;
			goto bad;
		}
		if (!rc)
			rc = bnxt_qplib_post_send(&qp->qplib_qp, &wqe);
bad:
		if (rc) {
			dev_err(rdev_to_dev(qp->rdev),
				"post_send failed op:%#x qps = %#x rc = %d\n",
				wr->opcode, qp->qplib_qp.state, rc);
			*bad_wr = wr;
			break;
		}
		wr = wr->next;
	}
	bnxt_qplib_post_send_db(&qp->qplib_qp);
	bnxt_ud_qp_hw_stall_workaround(qp);
	spin_unlock_irqrestore(&qp->sq_lock, flags);

	return rc;
}