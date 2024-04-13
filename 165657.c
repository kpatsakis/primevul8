static long sg_compat_ioctl(struct file *filp, unsigned int cmd_in, unsigned long arg)
{
	void __user *p = compat_ptr(arg);
	Sg_device *sdp;
	Sg_fd *sfp;
	int ret;

	if ((!(sfp = (Sg_fd *) filp->private_data)) || (!(sdp = sfp->parentdp)))
		return -ENXIO;

	ret = sg_ioctl_common(filp, sdp, sfp, cmd_in, p);
	if (ret != -ENOIOCTLCMD)
		return ret;

	return scsi_compat_ioctl(sdp->device, cmd_in, p);
}