
static int io_ring_add_registered_fd(struct io_uring_task *tctx, int fd,
				     int start, int end)
{
	struct file *file;
	int offset;

	for (offset = start; offset < end; offset++) {
		offset = array_index_nospec(offset, IO_RINGFD_REG_MAX);
		if (tctx->registered_rings[offset])
			continue;

		file = fget(fd);
		if (!file) {
			return -EBADF;
		} else if (file->f_op != &io_uring_fops) {
			fput(file);
			return -EOPNOTSUPP;
		}
		tctx->registered_rings[offset] = file;
		return offset;
	}

	return -EBUSY;