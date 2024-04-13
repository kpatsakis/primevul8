
static int io_buffer_account_pin(struct io_ring_ctx *ctx, struct page **pages,
				 int nr_pages, struct io_mapped_ubuf *imu,
				 struct page **last_hpage)
{
	int i, ret;

	imu->acct_pages = 0;
	for (i = 0; i < nr_pages; i++) {
		if (!PageCompound(pages[i])) {
			imu->acct_pages++;
		} else {
			struct page *hpage;

			hpage = compound_head(pages[i]);
			if (hpage == *last_hpage)
				continue;
			*last_hpage = hpage;
			if (headpage_already_acct(ctx, pages, i, hpage))
				continue;
			imu->acct_pages += page_size(hpage) >> PAGE_SHIFT;
		}
	}

	if (!imu->acct_pages)
		return 0;

	ret = io_account_mem(ctx, imu->acct_pages);
	if (ret)
		imu->acct_pages = 0;
	return ret;