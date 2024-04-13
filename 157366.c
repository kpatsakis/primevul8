int bnxt_re_dereg_mr(struct ib_mr *ib_mr, struct ib_udata *udata)
{
	struct bnxt_re_mr *mr = container_of(ib_mr, struct bnxt_re_mr, ib_mr);
	struct bnxt_re_dev *rdev = mr->rdev;
	int rc;

	rc = bnxt_qplib_free_mrw(&rdev->qplib_res, &mr->qplib_mr);
	if (rc)
		dev_err(rdev_to_dev(rdev), "Dereg MR failed: %#x\n", rc);

	if (mr->pages) {
		rc = bnxt_qplib_free_fast_reg_page_list(&rdev->qplib_res,
							&mr->qplib_frpl);
		kfree(mr->pages);
		mr->npages = 0;
		mr->pages = NULL;
	}
	ib_umem_release(mr->ib_umem);

	kfree(mr);
	atomic_dec(&rdev->mr_count);
	return rc;
}