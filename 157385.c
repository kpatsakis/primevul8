int bnxt_re_create_srq(struct ib_srq *ib_srq,
		       struct ib_srq_init_attr *srq_init_attr,
		       struct ib_udata *udata)
{
	struct ib_pd *ib_pd = ib_srq->pd;
	struct bnxt_re_pd *pd = container_of(ib_pd, struct bnxt_re_pd, ib_pd);
	struct bnxt_re_dev *rdev = pd->rdev;
	struct bnxt_qplib_dev_attr *dev_attr = &rdev->dev_attr;
	struct bnxt_re_srq *srq =
		container_of(ib_srq, struct bnxt_re_srq, ib_srq);
	struct bnxt_qplib_nq *nq = NULL;
	int rc, entries;

	if (srq_init_attr->attr.max_wr >= dev_attr->max_srq_wqes) {
		dev_err(rdev_to_dev(rdev), "Create CQ failed - max exceeded");
		rc = -EINVAL;
		goto exit;
	}

	if (srq_init_attr->srq_type != IB_SRQT_BASIC) {
		rc = -EOPNOTSUPP;
		goto exit;
	}

	srq->rdev = rdev;
	srq->qplib_srq.pd = &pd->qplib_pd;
	srq->qplib_srq.dpi = &rdev->dpi_privileged;
	/* Allocate 1 more than what's provided so posting max doesn't
	 * mean empty
	 */
	entries = roundup_pow_of_two(srq_init_attr->attr.max_wr + 1);
	if (entries > dev_attr->max_srq_wqes + 1)
		entries = dev_attr->max_srq_wqes + 1;

	srq->qplib_srq.max_wqe = entries;
	srq->qplib_srq.max_sge = srq_init_attr->attr.max_sge;
	srq->qplib_srq.threshold = srq_init_attr->attr.srq_limit;
	srq->srq_limit = srq_init_attr->attr.srq_limit;
	srq->qplib_srq.eventq_hw_ring_id = rdev->nq[0].ring_id;
	nq = &rdev->nq[0];

	if (udata) {
		rc = bnxt_re_init_user_srq(rdev, pd, srq, udata);
		if (rc)
			goto fail;
	}

	rc = bnxt_qplib_create_srq(&rdev->qplib_res, &srq->qplib_srq);
	if (rc) {
		dev_err(rdev_to_dev(rdev), "Create HW SRQ failed!");
		goto fail;
	}

	if (udata) {
		struct bnxt_re_srq_resp resp;

		resp.srqid = srq->qplib_srq.id;
		rc = ib_copy_to_udata(udata, &resp, sizeof(resp));
		if (rc) {
			dev_err(rdev_to_dev(rdev), "SRQ copy to udata failed!");
			bnxt_qplib_destroy_srq(&rdev->qplib_res,
					       &srq->qplib_srq);
			goto fail;
		}
	}
	if (nq)
		nq->budget++;
	atomic_inc(&rdev->srq_count);

	return 0;

fail:
	ib_umem_release(srq->umem);
exit:
	return rc;
}