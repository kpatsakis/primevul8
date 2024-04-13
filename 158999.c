pipe_poll(struct file *filp, poll_table *wait)
{
	__poll_t mask;
	struct pipe_inode_info *pipe = filp->private_data;
	unsigned int head, tail;

	/* Epoll has some historical nasty semantics, this enables them */
	pipe->poll_usage = 1;

	/*
	 * Reading pipe state only -- no need for acquiring the semaphore.
	 *
	 * But because this is racy, the code has to add the
	 * entry to the poll table _first_ ..
	 */
	if (filp->f_mode & FMODE_READ)
		poll_wait(filp, &pipe->rd_wait, wait);
	if (filp->f_mode & FMODE_WRITE)
		poll_wait(filp, &pipe->wr_wait, wait);

	/*
	 * .. and only then can you do the racy tests. That way,
	 * if something changes and you got it wrong, the poll
	 * table entry will wake you up and fix it.
	 */
	head = READ_ONCE(pipe->head);
	tail = READ_ONCE(pipe->tail);

	mask = 0;
	if (filp->f_mode & FMODE_READ) {
		if (!pipe_empty(head, tail))
			mask |= EPOLLIN | EPOLLRDNORM;
		if (!pipe->writers && filp->f_version != pipe->w_counter)
			mask |= EPOLLHUP;
	}

	if (filp->f_mode & FMODE_WRITE) {
		if (!pipe_full(head, tail, pipe->max_usage))
			mask |= EPOLLOUT | EPOLLWRNORM;
		/*
		 * Most Unices do not set EPOLLERR for FIFOs but on Linux they
		 * behave exactly like pipes for poll().
		 */
		if (!pipe->readers)
			mask |= EPOLLERR;
	}

	return mask;
}