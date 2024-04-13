struct ib_mr *bnxt_re_alloc_mr(struct ib_pd *ib_pd, enum ib_mr_type type,
			       u32 max_num_sg, struct ib_udata *udata)
{
	struct bnxt_re_pd *pd = container_of(ib_pd, struct bnxt_re_pd, ib_pd);
	struct bnxt_re_dev *rdev = pd->rdev;
	struct bnxt_re_mr *mr = NULL;
	int rc;

	if (type != IB_MR_TYPE_MEM_REG) {
		dev_dbg(rdev_to_dev(rdev), "MR type 0x%x not supported", type);
		return ERR_PTR(-EINVAL);
	}
	if (max_num_sg > MAX_PBL_LVL_1_PGS)
		return ERR_PTR(-EINVAL);

	mr = kzalloc(sizeof(*mr), GFP_KERNEL);
	if (!mr)
		return ERR_PTR(-ENOMEM);

	mr->rdev = rdev;
	mr->qplib_mr.pd = &pd->qplib_pd;
	mr->qplib_mr.flags = BNXT_QPLIB_FR_PMR;
	mr->qplib_mr.type = CMDQ_ALLOCATE_MRW_MRW_FLAGS_PMR;

	rc = bnxt_qplib_alloc_mrw(&rdev->qplib_res, &mr->qplib_mr);
	if (rc)
		goto bail;

	mr->ib_mr.lkey = mr->qplib_mr.lkey;
	mr->ib_mr.rkey = mr->ib_mr.lkey;

	mr->pages = kcalloc(max_num_sg, sizeof(u64), GFP_KERNEL);
	if (!mr->pages) {
		rc = -ENOMEM;
		goto fail;
	}
	rc = bnxt_qplib_alloc_fast_reg_page_list(&rdev->qplib_res,
						 &mr->qplib_frpl, max_num_sg);
	if (rc) {
		dev_err(rdev_to_dev(rdev),
			"Failed to allocate HW FR page list");
		goto fail_mr;
	}

	atomic_inc(&rdev->mr_count);
	return &mr->ib_mr;

fail_mr:
	kfree(mr->pages);
fail:
	bnxt_qplib_free_mrw(&rdev->qplib_res, &mr->qplib_mr);
bail:
	kfree(mr);
	return ERR_PTR(rc);
}