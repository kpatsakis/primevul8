int bnxt_re_destroy_qp(struct ib_qp *ib_qp, struct ib_udata *udata)
{
	struct bnxt_re_qp *qp = container_of(ib_qp, struct bnxt_re_qp, ib_qp);
	struct bnxt_re_dev *rdev = qp->rdev;
	unsigned int flags;
	int rc;

	bnxt_qplib_flush_cqn_wq(&qp->qplib_qp);
	rc = bnxt_qplib_destroy_qp(&rdev->qplib_res, &qp->qplib_qp);
	if (rc) {
		dev_err(rdev_to_dev(rdev), "Failed to destroy HW QP");
		return rc;
	}

	if (rdma_is_kernel_res(&qp->ib_qp.res)) {
		flags = bnxt_re_lock_cqs(qp);
		bnxt_qplib_clean_qp(&qp->qplib_qp);
		bnxt_re_unlock_cqs(qp, flags);
	}

	bnxt_qplib_free_qp_res(&rdev->qplib_res, &qp->qplib_qp);

	if (ib_qp->qp_type == IB_QPT_GSI && rdev->qp1_sqp) {
		bnxt_qplib_destroy_ah(&rdev->qplib_res, &rdev->sqp_ah->qplib_ah,
				      false);

		bnxt_qplib_clean_qp(&qp->qplib_qp);
		rc = bnxt_qplib_destroy_qp(&rdev->qplib_res,
					   &rdev->qp1_sqp->qplib_qp);
		if (rc) {
			dev_err(rdev_to_dev(rdev),
				"Failed to destroy Shadow QP");
			return rc;
		}
		bnxt_qplib_free_qp_res(&rdev->qplib_res,
				       &rdev->qp1_sqp->qplib_qp);
		mutex_lock(&rdev->qp_lock);
		list_del(&rdev->qp1_sqp->list);
		atomic_dec(&rdev->qp_count);
		mutex_unlock(&rdev->qp_lock);

		kfree(rdev->sqp_ah);
		kfree(rdev->qp1_sqp);
		rdev->qp1_sqp = NULL;
		rdev->sqp_ah = NULL;
	}

	ib_umem_release(qp->rumem);
	ib_umem_release(qp->sumem);

	mutex_lock(&rdev->qp_lock);
	list_del(&qp->list);
	atomic_dec(&rdev->qp_count);
	mutex_unlock(&rdev->qp_lock);
	kfree(qp);
	return 0;
}