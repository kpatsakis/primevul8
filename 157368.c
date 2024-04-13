int bnxt_re_alloc_ucontext(struct ib_ucontext *ctx, struct ib_udata *udata)
{
	struct ib_device *ibdev = ctx->device;
	struct bnxt_re_ucontext *uctx =
		container_of(ctx, struct bnxt_re_ucontext, ib_uctx);
	struct bnxt_re_dev *rdev = to_bnxt_re_dev(ibdev, ibdev);
	struct bnxt_qplib_dev_attr *dev_attr = &rdev->dev_attr;
	struct bnxt_re_uctx_resp resp;
	u32 chip_met_rev_num = 0;
	int rc;

	dev_dbg(rdev_to_dev(rdev), "ABI version requested %u",
		ibdev->ops.uverbs_abi_ver);

	if (ibdev->ops.uverbs_abi_ver != BNXT_RE_ABI_VERSION) {
		dev_dbg(rdev_to_dev(rdev), " is different from the device %d ",
			BNXT_RE_ABI_VERSION);
		return -EPERM;
	}

	uctx->rdev = rdev;

	uctx->shpg = (void *)__get_free_page(GFP_KERNEL);
	if (!uctx->shpg) {
		rc = -ENOMEM;
		goto fail;
	}
	spin_lock_init(&uctx->sh_lock);

	resp.comp_mask = BNXT_RE_UCNTX_CMASK_HAVE_CCTX;
	chip_met_rev_num = rdev->chip_ctx.chip_num;
	chip_met_rev_num |= ((u32)rdev->chip_ctx.chip_rev & 0xFF) <<
			     BNXT_RE_CHIP_ID0_CHIP_REV_SFT;
	chip_met_rev_num |= ((u32)rdev->chip_ctx.chip_metal & 0xFF) <<
			     BNXT_RE_CHIP_ID0_CHIP_MET_SFT;
	resp.chip_id0 = chip_met_rev_num;
	/* Future extension of chip info */
	resp.chip_id1 = 0;
	/*Temp, Use xa_alloc instead */
	resp.dev_id = rdev->en_dev->pdev->devfn;
	resp.max_qp = rdev->qplib_ctx.qpc_count;
	resp.pg_size = PAGE_SIZE;
	resp.cqe_sz = sizeof(struct cq_base);
	resp.max_cqd = dev_attr->max_cq_wqes;
	resp.rsvd    = 0;

	rc = ib_copy_to_udata(udata, &resp, min(udata->outlen, sizeof(resp)));
	if (rc) {
		dev_err(rdev_to_dev(rdev), "Failed to copy user context");
		rc = -EFAULT;
		goto cfail;
	}

	return 0;
cfail:
	free_page((unsigned long)uctx->shpg);
	uctx->shpg = NULL;
fail:
	return rc;
}