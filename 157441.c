static void bnxt_re_create_fence_wqe(struct bnxt_re_pd *pd)
{
	struct bnxt_re_fence_data *fence = &pd->fence;
	struct ib_mr *ib_mr = &fence->mr->ib_mr;
	struct bnxt_qplib_swqe *wqe = &fence->bind_wqe;

	memset(wqe, 0, sizeof(*wqe));
	wqe->type = BNXT_QPLIB_SWQE_TYPE_BIND_MW;
	wqe->wr_id = BNXT_QPLIB_FENCE_WRID;
	wqe->flags |= BNXT_QPLIB_SWQE_FLAGS_SIGNAL_COMP;
	wqe->flags |= BNXT_QPLIB_SWQE_FLAGS_UC_FENCE;
	wqe->bind.zero_based = false;
	wqe->bind.parent_l_key = ib_mr->lkey;
	wqe->bind.va = (u64)(unsigned long)fence->va;
	wqe->bind.length = fence->size;
	wqe->bind.access_cntl = __from_ib_access_flags(IB_ACCESS_REMOTE_READ);
	wqe->bind.mw_type = SQ_BIND_MW_TYPE_TYPE1;

	/* Save the initial rkey in fence structure for now;
	 * wqe->bind.r_key will be set at (re)bind time.
	 */
	fence->bind_rkey = ib_inc_rkey(fence->mw->rkey);
}