sg_remove_device(struct device *cl_dev, struct class_interface *cl_intf)
{
	struct scsi_device *scsidp = to_scsi_device(cl_dev->parent);
	Sg_device *sdp = dev_get_drvdata(cl_dev);
	unsigned long iflags;
	Sg_fd *sfp;
	int val;

	if (!sdp)
		return;
	/* want sdp->detaching non-zero as soon as possible */
	val = atomic_inc_return(&sdp->detaching);
	if (val > 1)
		return; /* only want to do following once per device */

	SCSI_LOG_TIMEOUT(3, sg_printk(KERN_INFO, sdp,
				      "%s\n", __func__));

	read_lock_irqsave(&sdp->sfd_lock, iflags);
	list_for_each_entry(sfp, &sdp->sfds, sfd_siblings) {
		wake_up_interruptible_all(&sfp->read_wait);
		kill_fasync(&sfp->async_qp, SIGPOLL, POLL_HUP);
	}
	wake_up_interruptible_all(&sdp->open_wait);
	read_unlock_irqrestore(&sdp->sfd_lock, iflags);

	sysfs_remove_link(&scsidp->sdev_gendev.kobj, "generic");
	device_destroy(sg_sysfs_class, MKDEV(SCSI_GENERIC_MAJOR, sdp->index));
	cdev_del(sdp->cdev);
	sdp->cdev = NULL;

	kref_put(&sdp->d_ref, sg_device_destroy);
}