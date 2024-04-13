int bnxt_re_create_cq(struct ib_cq *ibcq, const struct ib_cq_init_attr *attr,
		      struct ib_udata *udata)
{
	struct bnxt_re_dev *rdev = to_bnxt_re_dev(ibcq->device, ibdev);
	struct bnxt_qplib_dev_attr *dev_attr = &rdev->dev_attr;
	struct bnxt_re_cq *cq = container_of(ibcq, struct bnxt_re_cq, ib_cq);
	int rc, entries;
	int cqe = attr->cqe;
	struct bnxt_qplib_nq *nq = NULL;
	unsigned int nq_alloc_cnt;

	/* Validate CQ fields */
	if (cqe < 1 || cqe > dev_attr->max_cq_wqes) {
		dev_err(rdev_to_dev(rdev), "Failed to create CQ -max exceeded");
		return -EINVAL;
	}

	cq->rdev = rdev;
	cq->qplib_cq.cq_handle = (u64)(unsigned long)(&cq->qplib_cq);

	entries = roundup_pow_of_two(cqe + 1);
	if (entries > dev_attr->max_cq_wqes + 1)
		entries = dev_attr->max_cq_wqes + 1;

	if (udata) {
		struct bnxt_re_cq_req req;
		struct bnxt_re_ucontext *uctx = rdma_udata_to_drv_context(
			udata, struct bnxt_re_ucontext, ib_uctx);
		if (ib_copy_from_udata(&req, udata, sizeof(req))) {
			rc = -EFAULT;
			goto fail;
		}

		cq->umem = ib_umem_get(udata, req.cq_va,
				       entries * sizeof(struct cq_base),
				       IB_ACCESS_LOCAL_WRITE, 1);
		if (IS_ERR(cq->umem)) {
			rc = PTR_ERR(cq->umem);
			goto fail;
		}
		cq->qplib_cq.sg_info.sglist = cq->umem->sg_head.sgl;
		cq->qplib_cq.sg_info.npages = ib_umem_num_pages(cq->umem);
		cq->qplib_cq.sg_info.nmap = cq->umem->nmap;
		cq->qplib_cq.dpi = &uctx->dpi;
	} else {
		cq->max_cql = min_t(u32, entries, MAX_CQL_PER_POLL);
		cq->cql = kcalloc(cq->max_cql, sizeof(struct bnxt_qplib_cqe),
				  GFP_KERNEL);
		if (!cq->cql) {
			rc = -ENOMEM;
			goto fail;
		}

		cq->qplib_cq.dpi = &rdev->dpi_privileged;
	}
	/*
	 * Allocating the NQ in a round robin fashion. nq_alloc_cnt is a
	 * used for getting the NQ index.
	 */
	nq_alloc_cnt = atomic_inc_return(&rdev->nq_alloc_cnt);
	nq = &rdev->nq[nq_alloc_cnt % (rdev->num_msix - 1)];
	cq->qplib_cq.max_wqe = entries;
	cq->qplib_cq.cnq_hw_ring_id = nq->ring_id;
	cq->qplib_cq.nq	= nq;

	rc = bnxt_qplib_create_cq(&rdev->qplib_res, &cq->qplib_cq);
	if (rc) {
		dev_err(rdev_to_dev(rdev), "Failed to create HW CQ");
		goto fail;
	}

	cq->ib_cq.cqe = entries;
	cq->cq_period = cq->qplib_cq.period;
	nq->budget++;

	atomic_inc(&rdev->cq_count);
	spin_lock_init(&cq->cq_lock);

	if (udata) {
		struct bnxt_re_cq_resp resp;

		resp.cqid = cq->qplib_cq.id;
		resp.tail = cq->qplib_cq.hwq.cons;
		resp.phase = cq->qplib_cq.period;
		resp.rsvd = 0;
		rc = ib_copy_to_udata(udata, &resp, sizeof(resp));
		if (rc) {
			dev_err(rdev_to_dev(rdev), "Failed to copy CQ udata");
			bnxt_qplib_destroy_cq(&rdev->qplib_res, &cq->qplib_cq);
			goto c2fail;
		}
	}

	return 0;

c2fail:
	ib_umem_release(cq->umem);
fail:
	kfree(cq->cql);
	return rc;
}