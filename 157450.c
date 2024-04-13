int bnxt_re_dealloc_mw(struct ib_mw *ib_mw)
{
	struct bnxt_re_mw *mw = container_of(ib_mw, struct bnxt_re_mw, ib_mw);
	struct bnxt_re_dev *rdev = mw->rdev;
	int rc;

	rc = bnxt_qplib_free_mrw(&rdev->qplib_res, &mw->qplib_mw);
	if (rc) {
		dev_err(rdev_to_dev(rdev), "Free MW failed: %#x\n", rc);
		return rc;
	}

	kfree(mw);
	atomic_dec(&rdev->mw_count);
	return rc;
}