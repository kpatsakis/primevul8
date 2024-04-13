void bnxt_re_destroy_ah(struct ib_ah *ib_ah, u32 flags)
{
	struct bnxt_re_ah *ah = container_of(ib_ah, struct bnxt_re_ah, ib_ah);
	struct bnxt_re_dev *rdev = ah->rdev;

	bnxt_qplib_destroy_ah(&rdev->qplib_res, &ah->qplib_ah,
			      !(flags & RDMA_DESTROY_AH_SLEEPABLE));
}