sg_read(struct file *filp, char __user *buf, size_t count, loff_t * ppos)
{
	Sg_device *sdp;
	Sg_fd *sfp;
	Sg_request *srp;
	int req_pack_id = -1;
	sg_io_hdr_t *hp;
	struct sg_header *old_hdr;
	int retval;

	/*
	 * This could cause a response to be stranded. Close the associated
	 * file descriptor to free up any resources being held.
	 */
	retval = sg_check_file_access(filp, __func__);
	if (retval)
		return retval;

	if ((!(sfp = (Sg_fd *) filp->private_data)) || (!(sdp = sfp->parentdp)))
		return -ENXIO;
	SCSI_LOG_TIMEOUT(3, sg_printk(KERN_INFO, sdp,
				      "sg_read: count=%d\n", (int) count));

	if (sfp->force_packid)
		retval = get_sg_io_pack_id(&req_pack_id, buf, count);
	if (retval)
		return retval;

	srp = sg_get_rq_mark(sfp, req_pack_id);
	if (!srp) {		/* now wait on packet to arrive */
		if (atomic_read(&sdp->detaching))
			return -ENODEV;
		if (filp->f_flags & O_NONBLOCK)
			return -EAGAIN;
		retval = wait_event_interruptible(sfp->read_wait,
			(atomic_read(&sdp->detaching) ||
			(srp = sg_get_rq_mark(sfp, req_pack_id))));
		if (atomic_read(&sdp->detaching))
			return -ENODEV;
		if (retval)
			/* -ERESTARTSYS as signal hit process */
			return retval;
	}
	if (srp->header.interface_id != '\0')
		return sg_new_read(sfp, buf, count, srp);

	hp = &srp->header;
	old_hdr = kzalloc(SZ_SG_HEADER, GFP_KERNEL);
	if (!old_hdr)
		return -ENOMEM;

	old_hdr->reply_len = (int) hp->timeout;
	old_hdr->pack_len = old_hdr->reply_len; /* old, strange behaviour */
	old_hdr->pack_id = hp->pack_id;
	old_hdr->twelve_byte =
	    ((srp->data.cmd_opcode >= 0xc0) && (12 == hp->cmd_len)) ? 1 : 0;
	old_hdr->target_status = hp->masked_status;
	old_hdr->host_status = hp->host_status;
	old_hdr->driver_status = hp->driver_status;
	if ((CHECK_CONDITION & hp->masked_status) ||
	    (DRIVER_SENSE & hp->driver_status))
		memcpy(old_hdr->sense_buffer, srp->sense_b,
		       sizeof (old_hdr->sense_buffer));
	switch (hp->host_status) {
	/* This setup of 'result' is for backward compatibility and is best
	   ignored by the user who should use target, host + driver status */
	case DID_OK:
	case DID_PASSTHROUGH:
	case DID_SOFT_ERROR:
		old_hdr->result = 0;
		break;
	case DID_NO_CONNECT:
	case DID_BUS_BUSY:
	case DID_TIME_OUT:
		old_hdr->result = EBUSY;
		break;
	case DID_BAD_TARGET:
	case DID_ABORT:
	case DID_PARITY:
	case DID_RESET:
	case DID_BAD_INTR:
		old_hdr->result = EIO;
		break;
	case DID_ERROR:
		old_hdr->result = (srp->sense_b[0] == 0 && 
				  hp->masked_status == GOOD) ? 0 : EIO;
		break;
	default:
		old_hdr->result = EIO;
		break;
	}

	/* Now copy the result back to the user buffer.  */
	if (count >= SZ_SG_HEADER) {
		if (copy_to_user(buf, old_hdr, SZ_SG_HEADER)) {
			retval = -EFAULT;
			goto free_old_hdr;
		}
		buf += SZ_SG_HEADER;
		if (count > old_hdr->reply_len)
			count = old_hdr->reply_len;
		if (count > SZ_SG_HEADER) {
			if (sg_read_oxfer(srp, buf, count - SZ_SG_HEADER)) {
				retval = -EFAULT;
				goto free_old_hdr;
			}
		}
	} else
		count = (old_hdr->result == 0) ? 0 : -EIO;
	sg_finish_rem_req(srp);
	sg_remove_request(sfp, srp);
	retval = count;
free_old_hdr:
	kfree(old_hdr);
	return retval;
}