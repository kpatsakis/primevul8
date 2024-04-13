pipe_write(struct kiocb *iocb, struct iov_iter *from)
{
	struct file *filp = iocb->ki_filp;
	struct pipe_inode_info *pipe = filp->private_data;
	unsigned int head;
	ssize_t ret = 0;
	size_t total_len = iov_iter_count(from);
	ssize_t chars;
	bool was_empty = false;
	bool wake_next_writer = false;

	/* Null write succeeds. */
	if (unlikely(total_len == 0))
		return 0;

	__pipe_lock(pipe);

	if (!pipe->readers) {
		send_sig(SIGPIPE, current, 0);
		ret = -EPIPE;
		goto out;
	}

#ifdef CONFIG_WATCH_QUEUE
	if (pipe->watch_queue) {
		ret = -EXDEV;
		goto out;
	}
#endif

	/*
	 * If it wasn't empty we try to merge new data into
	 * the last buffer.
	 *
	 * That naturally merges small writes, but it also
	 * page-aligns the rest of the writes for large writes
	 * spanning multiple pages.
	 */
	head = pipe->head;
	was_empty = pipe_empty(head, pipe->tail);
	chars = total_len & (PAGE_SIZE-1);
	if (chars && !was_empty) {
		unsigned int mask = pipe->ring_size - 1;
		struct pipe_buffer *buf = &pipe->bufs[(head - 1) & mask];
		int offset = buf->offset + buf->len;

		if ((buf->flags & PIPE_BUF_FLAG_CAN_MERGE) &&
		    offset + chars <= PAGE_SIZE) {
			ret = pipe_buf_confirm(pipe, buf);
			if (ret)
				goto out;

			ret = copy_page_from_iter(buf->page, offset, chars, from);
			if (unlikely(ret < chars)) {
				ret = -EFAULT;
				goto out;
			}

			buf->len += ret;
			if (!iov_iter_count(from))
				goto out;
		}
	}

	for (;;) {
		if (!pipe->readers) {
			send_sig(SIGPIPE, current, 0);
			if (!ret)
				ret = -EPIPE;
			break;
		}

		head = pipe->head;
		if (!pipe_full(head, pipe->tail, pipe->max_usage)) {
			unsigned int mask = pipe->ring_size - 1;
			struct pipe_buffer *buf = &pipe->bufs[head & mask];
			struct page *page = pipe->tmp_page;
			int copied;

			if (!page) {
				page = alloc_page(GFP_HIGHUSER | __GFP_ACCOUNT);
				if (unlikely(!page)) {
					ret = ret ? : -ENOMEM;
					break;
				}
				pipe->tmp_page = page;
			}

			/* Allocate a slot in the ring in advance and attach an
			 * empty buffer.  If we fault or otherwise fail to use
			 * it, either the reader will consume it or it'll still
			 * be there for the next write.
			 */
			spin_lock_irq(&pipe->rd_wait.lock);

			head = pipe->head;
			if (pipe_full(head, pipe->tail, pipe->max_usage)) {
				spin_unlock_irq(&pipe->rd_wait.lock);
				continue;
			}

			pipe->head = head + 1;
			spin_unlock_irq(&pipe->rd_wait.lock);

			/* Insert it into the buffer array */
			buf = &pipe->bufs[head & mask];
			buf->page = page;
			buf->ops = &anon_pipe_buf_ops;
			buf->offset = 0;
			buf->len = 0;
			if (is_packetized(filp))
				buf->flags = PIPE_BUF_FLAG_PACKET;
			else
				buf->flags = PIPE_BUF_FLAG_CAN_MERGE;
			pipe->tmp_page = NULL;

			copied = copy_page_from_iter(page, 0, PAGE_SIZE, from);
			if (unlikely(copied < PAGE_SIZE && iov_iter_count(from))) {
				if (!ret)
					ret = -EFAULT;
				break;
			}
			ret += copied;
			buf->offset = 0;
			buf->len = copied;

			if (!iov_iter_count(from))
				break;
		}

		if (!pipe_full(head, pipe->tail, pipe->max_usage))
			continue;

		/* Wait for buffer space to become available. */
		if (filp->f_flags & O_NONBLOCK) {
			if (!ret)
				ret = -EAGAIN;
			break;
		}
		if (signal_pending(current)) {
			if (!ret)
				ret = -ERESTARTSYS;
			break;
		}

		/*
		 * We're going to release the pipe lock and wait for more
		 * space. We wake up any readers if necessary, and then
		 * after waiting we need to re-check whether the pipe
		 * become empty while we dropped the lock.
		 */
		__pipe_unlock(pipe);
		if (was_empty)
			wake_up_interruptible_sync_poll(&pipe->rd_wait, EPOLLIN | EPOLLRDNORM);
		kill_fasync(&pipe->fasync_readers, SIGIO, POLL_IN);
		wait_event_interruptible_exclusive(pipe->wr_wait, pipe_writable(pipe));
		__pipe_lock(pipe);
		was_empty = pipe_empty(pipe->head, pipe->tail);
		wake_next_writer = true;
	}
out:
	if (pipe_full(pipe->head, pipe->tail, pipe->max_usage))
		wake_next_writer = false;
	__pipe_unlock(pipe);

	/*
	 * If we do do a wakeup event, we do a 'sync' wakeup, because we
	 * want the reader to start processing things asap, rather than
	 * leave the data pending.
	 *
	 * This is particularly important for small writes, because of
	 * how (for example) the GNU make jobserver uses small writes to
	 * wake up pending jobs
	 *
	 * Epoll nonsensically wants a wakeup whether the pipe
	 * was already empty or not.
	 */
	if (was_empty || pipe->poll_usage)
		wake_up_interruptible_sync_poll(&pipe->rd_wait, EPOLLIN | EPOLLRDNORM);
	kill_fasync(&pipe->fasync_readers, SIGIO, POLL_IN);
	if (wake_next_writer)
		wake_up_interruptible_sync_poll(&pipe->wr_wait, EPOLLOUT | EPOLLWRNORM);
	if (ret > 0 && sb_start_write_trylock(file_inode(filp)->i_sb)) {
		int err = file_update_time(filp);
		if (err)
			ret = err;
		sb_end_write(file_inode(filp)->i_sb);
	}
	return ret;
}