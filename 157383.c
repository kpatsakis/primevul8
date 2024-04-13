static void bnxt_re_destroy_fence_mr(struct bnxt_re_pd *pd)
{
	struct bnxt_re_fence_data *fence = &pd->fence;
	struct bnxt_re_dev *rdev = pd->rdev;
	struct device *dev = &rdev->en_dev->pdev->dev;
	struct bnxt_re_mr *mr = fence->mr;

	if (fence->mw) {
		bnxt_re_dealloc_mw(fence->mw);
		fence->mw = NULL;
	}
	if (mr) {
		if (mr->ib_mr.rkey)
			bnxt_qplib_dereg_mrw(&rdev->qplib_res, &mr->qplib_mr,
					     true);
		if (mr->ib_mr.lkey)
			bnxt_qplib_free_mrw(&rdev->qplib_res, &mr->qplib_mr);
		kfree(mr);
		fence->mr = NULL;
	}
	if (fence->dma_addr) {
		dma_unmap_single(dev, fence->dma_addr, BNXT_RE_FENCE_BYTES,
				 DMA_BIDIRECTIONAL);
		fence->dma_addr = 0;
	}
}