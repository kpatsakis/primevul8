struct ib_mr *bnxt_re_reg_user_mr(struct ib_pd *ib_pd, u64 start, u64 length,
				  u64 virt_addr, int mr_access_flags,
				  struct ib_udata *udata)
{
	struct bnxt_re_pd *pd = container_of(ib_pd, struct bnxt_re_pd, ib_pd);
	struct bnxt_re_dev *rdev = pd->rdev;
	struct bnxt_re_mr *mr;
	struct ib_umem *umem;
	u64 *pbl_tbl = NULL;
	int umem_pgs, page_shift, rc;

	if (length > BNXT_RE_MAX_MR_SIZE) {
		dev_err(rdev_to_dev(rdev), "MR Size: %lld > Max supported:%lld\n",
			length, BNXT_RE_MAX_MR_SIZE);
		return ERR_PTR(-ENOMEM);
	}

	mr = kzalloc(sizeof(*mr), GFP_KERNEL);
	if (!mr)
		return ERR_PTR(-ENOMEM);

	mr->rdev = rdev;
	mr->qplib_mr.pd = &pd->qplib_pd;
	mr->qplib_mr.flags = __from_ib_access_flags(mr_access_flags);
	mr->qplib_mr.type = CMDQ_ALLOCATE_MRW_MRW_FLAGS_MR;

	rc = bnxt_qplib_alloc_mrw(&rdev->qplib_res, &mr->qplib_mr);
	if (rc) {
		dev_err(rdev_to_dev(rdev), "Failed to allocate MR");
		goto free_mr;
	}
	/* The fixed portion of the rkey is the same as the lkey */
	mr->ib_mr.rkey = mr->qplib_mr.rkey;

	umem = ib_umem_get(udata, start, length, mr_access_flags, 0);
	if (IS_ERR(umem)) {
		dev_err(rdev_to_dev(rdev), "Failed to get umem");
		rc = -EFAULT;
		goto free_mrw;
	}
	mr->ib_umem = umem;

	mr->qplib_mr.va = virt_addr;
	umem_pgs = ib_umem_page_count(umem);
	if (!umem_pgs) {
		dev_err(rdev_to_dev(rdev), "umem is invalid!");
		rc = -EINVAL;
		goto free_umem;
	}
	mr->qplib_mr.total_size = length;

	pbl_tbl = kcalloc(umem_pgs, sizeof(u64 *), GFP_KERNEL);
	if (!pbl_tbl) {
		rc = -ENOMEM;
		goto free_umem;
	}

	page_shift = __ffs(ib_umem_find_best_pgsz(umem,
				BNXT_RE_PAGE_SIZE_4K | BNXT_RE_PAGE_SIZE_2M,
				virt_addr));

	if (!bnxt_re_page_size_ok(page_shift)) {
		dev_err(rdev_to_dev(rdev), "umem page size unsupported!");
		rc = -EFAULT;
		goto fail;
	}

	if (page_shift == BNXT_RE_PAGE_SHIFT_4K &&
	    length > BNXT_RE_MAX_MR_SIZE_LOW) {
		dev_err(rdev_to_dev(rdev), "Requested MR Sz:%llu Max sup:%llu",
			length,	(u64)BNXT_RE_MAX_MR_SIZE_LOW);
		rc = -EINVAL;
		goto fail;
	}

	/* Map umem buf ptrs to the PBL */
	umem_pgs = fill_umem_pbl_tbl(umem, pbl_tbl, page_shift);
	rc = bnxt_qplib_reg_mr(&rdev->qplib_res, &mr->qplib_mr, pbl_tbl,
			       umem_pgs, false, 1 << page_shift);
	if (rc) {
		dev_err(rdev_to_dev(rdev), "Failed to register user MR");
		goto fail;
	}

	kfree(pbl_tbl);

	mr->ib_mr.lkey = mr->qplib_mr.lkey;
	mr->ib_mr.rkey = mr->qplib_mr.lkey;
	atomic_inc(&rdev->mr_count);

	return &mr->ib_mr;
fail:
	kfree(pbl_tbl);
free_umem:
	ib_umem_release(umem);
free_mrw:
	bnxt_qplib_free_mrw(&rdev->qplib_res, &mr->qplib_mr);
free_mr:
	kfree(mr);
	return ERR_PTR(rc);
}