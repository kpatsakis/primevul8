struct ib_mr *bnxt_re_get_dma_mr(struct ib_pd *ib_pd, int mr_access_flags)
{
	struct bnxt_re_pd *pd = container_of(ib_pd, struct bnxt_re_pd, ib_pd);
	struct bnxt_re_dev *rdev = pd->rdev;
	struct bnxt_re_mr *mr;
	u64 pbl = 0;
	int rc;

	mr = kzalloc(sizeof(*mr), GFP_KERNEL);
	if (!mr)
		return ERR_PTR(-ENOMEM);

	mr->rdev = rdev;
	mr->qplib_mr.pd = &pd->qplib_pd;
	mr->qplib_mr.flags = __from_ib_access_flags(mr_access_flags);
	mr->qplib_mr.type = CMDQ_ALLOCATE_MRW_MRW_FLAGS_PMR;

	/* Allocate and register 0 as the address */
	rc = bnxt_qplib_alloc_mrw(&rdev->qplib_res, &mr->qplib_mr);
	if (rc)
		goto fail;

	mr->qplib_mr.hwq.level = PBL_LVL_MAX;
	mr->qplib_mr.total_size = -1; /* Infinte length */
	rc = bnxt_qplib_reg_mr(&rdev->qplib_res, &mr->qplib_mr, &pbl, 0, false,
			       PAGE_SIZE);
	if (rc)
		goto fail_mr;

	mr->ib_mr.lkey = mr->qplib_mr.lkey;
	if (mr_access_flags & (IB_ACCESS_REMOTE_WRITE | IB_ACCESS_REMOTE_READ |
			       IB_ACCESS_REMOTE_ATOMIC))
		mr->ib_mr.rkey = mr->ib_mr.lkey;
	atomic_inc(&rdev->mr_count);

	return &mr->ib_mr;

fail_mr:
	bnxt_qplib_free_mrw(&rdev->qplib_res, &mr->qplib_mr);
fail:
	kfree(mr);
	return ERR_PTR(rc);
}