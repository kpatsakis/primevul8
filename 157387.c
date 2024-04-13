int bnxt_re_map_mr_sg(struct ib_mr *ib_mr, struct scatterlist *sg, int sg_nents,
		      unsigned int *sg_offset)
{
	struct bnxt_re_mr *mr = container_of(ib_mr, struct bnxt_re_mr, ib_mr);

	mr->npages = 0;
	return ib_sg_to_pages(ib_mr, sg, sg_nents, sg_offset, bnxt_re_set_page);
}