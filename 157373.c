static int bnxt_re_create_fence_mr(struct bnxt_re_pd *pd)
{
	int mr_access_flags = IB_ACCESS_LOCAL_WRITE | IB_ACCESS_MW_BIND;
	struct bnxt_re_fence_data *fence = &pd->fence;
	struct bnxt_re_dev *rdev = pd->rdev;
	struct device *dev = &rdev->en_dev->pdev->dev;
	struct bnxt_re_mr *mr = NULL;
	dma_addr_t dma_addr = 0;
	struct ib_mw *mw;
	u64 pbl_tbl;
	int rc;

	dma_addr = dma_map_single(dev, fence->va, BNXT_RE_FENCE_BYTES,
				  DMA_BIDIRECTIONAL);
	rc = dma_mapping_error(dev, dma_addr);
	if (rc) {
		dev_err(rdev_to_dev(rdev), "Failed to dma-map fence-MR-mem\n");
		rc = -EIO;
		fence->dma_addr = 0;
		goto fail;
	}
	fence->dma_addr = dma_addr;

	/* Allocate a MR */
	mr = kzalloc(sizeof(*mr), GFP_KERNEL);
	if (!mr) {
		rc = -ENOMEM;
		goto fail;
	}
	fence->mr = mr;
	mr->rdev = rdev;
	mr->qplib_mr.pd = &pd->qplib_pd;
	mr->qplib_mr.type = CMDQ_ALLOCATE_MRW_MRW_FLAGS_PMR;
	mr->qplib_mr.flags = __from_ib_access_flags(mr_access_flags);
	rc = bnxt_qplib_alloc_mrw(&rdev->qplib_res, &mr->qplib_mr);
	if (rc) {
		dev_err(rdev_to_dev(rdev), "Failed to alloc fence-HW-MR\n");
		goto fail;
	}

	/* Register MR */
	mr->ib_mr.lkey = mr->qplib_mr.lkey;
	mr->qplib_mr.va = (u64)(unsigned long)fence->va;
	mr->qplib_mr.total_size = BNXT_RE_FENCE_BYTES;
	pbl_tbl = dma_addr;
	rc = bnxt_qplib_reg_mr(&rdev->qplib_res, &mr->qplib_mr, &pbl_tbl,
			       BNXT_RE_FENCE_PBL_SIZE, false, PAGE_SIZE);
	if (rc) {
		dev_err(rdev_to_dev(rdev), "Failed to register fence-MR\n");
		goto fail;
	}
	mr->ib_mr.rkey = mr->qplib_mr.rkey;

	/* Create a fence MW only for kernel consumers */
	mw = bnxt_re_alloc_mw(&pd->ib_pd, IB_MW_TYPE_1, NULL);
	if (IS_ERR(mw)) {
		dev_err(rdev_to_dev(rdev),
			"Failed to create fence-MW for PD: %p\n", pd);
		rc = PTR_ERR(mw);
		goto fail;
	}
	fence->mw = mw;

	bnxt_re_create_fence_wqe(pd);
	return 0;

fail:
	bnxt_re_destroy_fence_mr(pd);
	return rc;
}