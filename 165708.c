sg_poll(struct file *filp, poll_table * wait)
{
	__poll_t res = 0;
	Sg_device *sdp;
	Sg_fd *sfp;
	Sg_request *srp;
	int count = 0;
	unsigned long iflags;

	sfp = filp->private_data;
	if (!sfp)
		return EPOLLERR;
	sdp = sfp->parentdp;
	if (!sdp)
		return EPOLLERR;
	poll_wait(filp, &sfp->read_wait, wait);
	read_lock_irqsave(&sfp->rq_list_lock, iflags);
	list_for_each_entry(srp, &sfp->rq_list, entry) {
		/* if any read waiting, flag it */
		if ((0 == res) && (1 == srp->done) && (!srp->sg_io_owned))
			res = EPOLLIN | EPOLLRDNORM;
		++count;
	}
	read_unlock_irqrestore(&sfp->rq_list_lock, iflags);

	if (atomic_read(&sdp->detaching))
		res |= EPOLLHUP;
	else if (!sfp->cmd_q) {
		if (0 == count)
			res |= EPOLLOUT | EPOLLWRNORM;
	} else if (count < SG_MAX_QUEUE)
		res |= EPOLLOUT | EPOLLWRNORM;
	SCSI_LOG_TIMEOUT(3, sg_printk(KERN_INFO, sdp,
				      "sg_poll: res=0x%x\n", (__force u32) res));
	return res;
}