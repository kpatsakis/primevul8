static int __floppy_read_block_0(struct block_device *bdev, int drive)
{
	struct bio bio;
	struct bio_vec bio_vec;
	struct page *page;
	struct rb0_cbdata cbdata;
	size_t size;

	page = alloc_page(GFP_NOIO);
	if (!page) {
		process_fd_request();
		return -ENOMEM;
	}

	size = bdev->bd_block_size;
	if (!size)
		size = 1024;

	cbdata.drive = drive;

	bio_init(&bio, &bio_vec, 1);
	bio_set_dev(&bio, bdev);
	bio_add_page(&bio, page, size, 0);

	bio.bi_iter.bi_sector = 0;
	bio.bi_flags |= (1 << BIO_QUIET);
	bio.bi_private = &cbdata;
	bio.bi_end_io = floppy_rb0_cb;
	bio_set_op_attrs(&bio, REQ_OP_READ, 0);

	init_completion(&cbdata.complete);

	submit_bio(&bio);
	process_fd_request();

	wait_for_completion(&cbdata.complete);

	__free_page(page);

	return 0;
}