static int bnxt_re_set_page(struct ib_mr *ib_mr, u64 addr)
{
	struct bnxt_re_mr *mr = container_of(ib_mr, struct bnxt_re_mr, ib_mr);

	if (unlikely(mr->npages == mr->qplib_frpl.max_pg_ptrs))
		return -ENOMEM;

	mr->pages[mr->npages++] = addr;
	return 0;
}