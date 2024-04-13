
static int io_sqe_buffer_register(struct io_ring_ctx *ctx, struct iovec *iov,
				  struct io_mapped_ubuf **pimu,
				  struct page **last_hpage)
{
	struct io_mapped_ubuf *imu = NULL;
	struct vm_area_struct **vmas = NULL;
	struct page **pages = NULL;
	unsigned long off, start, end, ubuf;
	size_t size;
	int ret, pret, nr_pages, i;

	if (!iov->iov_base) {
		*pimu = ctx->dummy_ubuf;
		return 0;
	}

	ubuf = (unsigned long) iov->iov_base;
	end = (ubuf + iov->iov_len + PAGE_SIZE - 1) >> PAGE_SHIFT;
	start = ubuf >> PAGE_SHIFT;
	nr_pages = end - start;

	*pimu = NULL;
	ret = -ENOMEM;

	pages = kvmalloc_array(nr_pages, sizeof(struct page *), GFP_KERNEL);
	if (!pages)
		goto done;

	vmas = kvmalloc_array(nr_pages, sizeof(struct vm_area_struct *),
			      GFP_KERNEL);
	if (!vmas)
		goto done;

	imu = kvmalloc(struct_size(imu, bvec, nr_pages), GFP_KERNEL);
	if (!imu)
		goto done;

	ret = 0;
	mmap_read_lock(current->mm);
	pret = pin_user_pages(ubuf, nr_pages, FOLL_WRITE | FOLL_LONGTERM,
			      pages, vmas);
	if (pret == nr_pages) {
		/* don't support file backed memory */
		for (i = 0; i < nr_pages; i++) {
			struct vm_area_struct *vma = vmas[i];

			if (vma_is_shmem(vma))
				continue;
			if (vma->vm_file &&
			    !is_file_hugepages(vma->vm_file)) {
				ret = -EOPNOTSUPP;
				break;
			}
		}
	} else {
		ret = pret < 0 ? pret : -EFAULT;
	}
	mmap_read_unlock(current->mm);
	if (ret) {
		/*
		 * if we did partial map, or found file backed vmas,
		 * release any pages we did get
		 */
		if (pret > 0)
			unpin_user_pages(pages, pret);
		goto done;
	}

	ret = io_buffer_account_pin(ctx, pages, pret, imu, last_hpage);
	if (ret) {
		unpin_user_pages(pages, pret);
		goto done;
	}

	off = ubuf & ~PAGE_MASK;
	size = iov->iov_len;
	for (i = 0; i < nr_pages; i++) {
		size_t vec_len;

		vec_len = min_t(size_t, size, PAGE_SIZE - off);
		imu->bvec[i].bv_page = pages[i];
		imu->bvec[i].bv_len = vec_len;
		imu->bvec[i].bv_offset = off;
		off = 0;
		size -= vec_len;
	}
	/* store original address for later verification */
	imu->ubuf = ubuf;
	imu->ubuf_end = ubuf + iov->iov_len;
	imu->nr_bvecs = nr_pages;
	*pimu = imu;
	ret = 0;
done:
	if (ret)
		kvfree(imu);
	kvfree(pages);
	kvfree(vmas);
	return ret;