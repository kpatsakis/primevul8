void bnxt_re_dealloc_pd(struct ib_pd *ib_pd, struct ib_udata *udata)
{
	struct bnxt_re_pd *pd = container_of(ib_pd, struct bnxt_re_pd, ib_pd);
	struct bnxt_re_dev *rdev = pd->rdev;

	bnxt_re_destroy_fence_mr(pd);

	if (pd->qplib_pd.id)
		bnxt_qplib_dealloc_pd(&rdev->qplib_res, &rdev->qplib_res.pd_tbl,
				      &pd->qplib_pd);
}