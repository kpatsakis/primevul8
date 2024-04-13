int bnxt_re_alloc_pd(struct ib_pd *ibpd, struct ib_udata *udata)
{
	struct ib_device *ibdev = ibpd->device;
	struct bnxt_re_dev *rdev = to_bnxt_re_dev(ibdev, ibdev);
	struct bnxt_re_ucontext *ucntx = rdma_udata_to_drv_context(
		udata, struct bnxt_re_ucontext, ib_uctx);
	struct bnxt_re_pd *pd = container_of(ibpd, struct bnxt_re_pd, ib_pd);
	int rc;

	pd->rdev = rdev;
	if (bnxt_qplib_alloc_pd(&rdev->qplib_res.pd_tbl, &pd->qplib_pd)) {
		dev_err(rdev_to_dev(rdev), "Failed to allocate HW PD");
		rc = -ENOMEM;
		goto fail;
	}

	if (udata) {
		struct bnxt_re_pd_resp resp;

		if (!ucntx->dpi.dbr) {
			/* Allocate DPI in alloc_pd to avoid failing of
			 * ibv_devinfo and family of application when DPIs
			 * are depleted.
			 */
			if (bnxt_qplib_alloc_dpi(&rdev->qplib_res.dpi_tbl,
						 &ucntx->dpi, ucntx)) {
				rc = -ENOMEM;
				goto dbfail;
			}
		}

		resp.pdid = pd->qplib_pd.id;
		/* Still allow mapping this DBR to the new user PD. */
		resp.dpi = ucntx->dpi.dpi;
		resp.dbr = (u64)ucntx->dpi.umdbr;

		rc = ib_copy_to_udata(udata, &resp, sizeof(resp));
		if (rc) {
			dev_err(rdev_to_dev(rdev),
				"Failed to copy user response\n");
			goto dbfail;
		}
	}

	if (!udata)
		if (bnxt_re_create_fence_mr(pd))
			dev_warn(rdev_to_dev(rdev),
				 "Failed to create Fence-MR\n");
	return 0;
dbfail:
	bnxt_qplib_dealloc_pd(&rdev->qplib_res, &rdev->qplib_res.pd_tbl,
			      &pd->qplib_pd);
fail:
	return rc;
}