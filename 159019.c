pipe_read(struct kiocb *iocb, struct iov_iter *to)
{
	size_t total_len = iov_iter_count(to);
	struct file *filp = iocb->ki_filp;
	struct pipe_inode_info *pipe = filp->private_data;
	bool was_full, wake_next_reader = false;
	ssize_t ret;

	/* Null read succeeds. */
	if (unlikely(total_len == 0))
		return 0;

	ret = 0;
	__pipe_lock(pipe);

	/*
	 * We only wake up writers if the pipe was full when we started
	 * reading in order to avoid unnecessary wakeups.
	 *
	 * But when we do wake up writers, we do so using a sync wakeup
	 * (WF_SYNC), because we want them to get going and generate more
	 * data for us.
	 */
	was_full = pipe_full(pipe->head, pipe->tail, pipe->max_usage);
	for (;;) {
		/* Read ->head with a barrier vs post_one_notification() */
		unsigned int head = smp_load_acquire(&pipe->head);
		unsigned int tail = pipe->tail;
		unsigned int mask = pipe->ring_size - 1;

#ifdef CONFIG_WATCH_QUEUE
		if (pipe->note_loss) {
			struct watch_notification n;

			if (total_len < 8) {
				if (ret == 0)
					ret = -ENOBUFS;
				break;
			}

			n.type = WATCH_TYPE_META;
			n.subtype = WATCH_META_LOSS_NOTIFICATION;
			n.info = watch_sizeof(n);
			if (copy_to_iter(&n, sizeof(n), to) != sizeof(n)) {
				if (ret == 0)
					ret = -EFAULT;
				break;
			}
			ret += sizeof(n);
			total_len -= sizeof(n);
			pipe->note_loss = false;
		}
#endif

		if (!pipe_empty(head, tail)) {
			struct pipe_buffer *buf = &pipe->bufs[tail & mask];
			size_t chars = buf->len;
			size_t written;
			int error;

			if (chars > total_len) {
				if (buf->flags & PIPE_BUF_FLAG_WHOLE) {
					if (ret == 0)
						ret = -ENOBUFS;
					break;
				}
				chars = total_len;
			}

			error = pipe_buf_confirm(pipe, buf);
			if (error) {
				if (!ret)
					ret = error;
				break;
			}

			written = copy_page_to_iter(buf->page, buf->offset, chars, to);
			if (unlikely(written < chars)) {
				if (!ret)
					ret = -EFAULT;
				break;
			}
			ret += chars;
			buf->offset += chars;
			buf->len -= chars;

			/* Was it a packet buffer? Clean up and exit */
			if (buf->flags & PIPE_BUF_FLAG_PACKET) {
				total_len = chars;
				buf->len = 0;
			}

			if (!buf->len) {
				pipe_buf_release(pipe, buf);
				spin_lock_irq(&pipe->rd_wait.lock);
#ifdef CONFIG_WATCH_QUEUE
				if (buf->flags & PIPE_BUF_FLAG_LOSS)
					pipe->note_loss = true;
#endif
				tail++;
				pipe->tail = tail;
				spin_unlock_irq(&pipe->rd_wait.lock);
			}
			total_len -= chars;
			if (!total_len)
				break;	/* common path: read succeeded */
			if (!pipe_empty(head, tail))	/* More to do? */
				continue;
		}

		if (!pipe->writers)
			break;
		if (ret)
			break;
		if (filp->f_flags & O_NONBLOCK) {
			ret = -EAGAIN;
			break;
		}
		__pipe_unlock(pipe);

		/*
		 * We only get here if we didn't actually read anything.
		 *
		 * However, we could have seen (and removed) a zero-sized
		 * pipe buffer, and might have made space in the buffers
		 * that way.
		 *
		 * You can't make zero-sized pipe buffers by doing an empty
		 * write (not even in packet mode), but they can happen if
		 * the writer gets an EFAULT when trying to fill a buffer
		 * that already got allocated and inserted in the buffer
		 * array.
		 *
		 * So we still need to wake up any pending writers in the
		 * _very_ unlikely case that the pipe was full, but we got
		 * no data.
		 */
		if (unlikely(was_full))
			wake_up_interruptible_sync_poll(&pipe->wr_wait, EPOLLOUT | EPOLLWRNORM);
		kill_fasync(&pipe->fasync_writers, SIGIO, POLL_OUT);

		/*
		 * But because we didn't read anything, at this point we can
		 * just return directly with -ERESTARTSYS if we're interrupted,
		 * since we've done any required wakeups and there's no need
		 * to mark anything accessed. And we've dropped the lock.
		 */
		if (wait_event_interruptible_exclusive(pipe->rd_wait, pipe_readable(pipe)) < 0)
			return -ERESTARTSYS;

		__pipe_lock(pipe);
		was_full = pipe_full(pipe->head, pipe->tail, pipe->max_usage);
		wake_next_reader = true;
	}
	if (pipe_empty(pipe->head, pipe->tail))
		wake_next_reader = false;
	__pipe_unlock(pipe);

	if (was_full)
		wake_up_interruptible_sync_poll(&pipe->wr_wait, EPOLLOUT | EPOLLWRNORM);
	if (wake_next_reader)
		wake_up_interruptible_sync_poll(&pipe->rd_wait, EPOLLIN | EPOLLRDNORM);
	kill_fasync(&pipe->fasync_writers, SIGIO, POLL_OUT);
	if (ret > 0)
		file_accessed(filp);
	return ret;
}