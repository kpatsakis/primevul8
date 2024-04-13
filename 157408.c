static int fill_umem_pbl_tbl(struct ib_umem *umem, u64 *pbl_tbl_orig,
			     int page_shift)
{
	u64 *pbl_tbl = pbl_tbl_orig;
	u64 page_size =  BIT_ULL(page_shift);
	struct ib_block_iter biter;

	rdma_for_each_block(umem->sg_head.sgl, &biter, umem->nmap, page_size)
		*pbl_tbl++ = rdma_block_iter_dma_address(&biter);

	return pbl_tbl - pbl_tbl_orig;
}