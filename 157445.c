struct ib_mw *bnxt_re_alloc_mw(struct ib_pd *ib_pd, enum ib_mw_type type,
			       struct ib_udata *udata)
{
	struct bnxt_re_pd *pd = container_of(ib_pd, struct bnxt_re_pd, ib_pd);
	struct bnxt_re_dev *rdev = pd->rdev;
	struct bnxt_re_mw *mw;
	int rc;

	mw = kzalloc(sizeof(*mw), GFP_KERNEL);
	if (!mw)
		return ERR_PTR(-ENOMEM);
	mw->rdev = rdev;
	mw->qplib_mw.pd = &pd->qplib_pd;

	mw->qplib_mw.type = (type == IB_MW_TYPE_1 ?
			       CMDQ_ALLOCATE_MRW_MRW_FLAGS_MW_TYPE1 :
			       CMDQ_ALLOCATE_MRW_MRW_FLAGS_MW_TYPE2B);
	rc = bnxt_qplib_alloc_mrw(&rdev->qplib_res, &mw->qplib_mw);
	if (rc) {
		dev_err(rdev_to_dev(rdev), "Allocate MW failed!");
		goto fail;
	}
	mw->ib_mw.rkey = mw->qplib_mw.rkey;

	atomic_inc(&rdev->mw_count);
	return &mw->ib_mw;

fail:
	kfree(mw);
	return ERR_PTR(rc);
}