static int snd_timer_user_status(struct file *file,
				 struct snd_timer_status __user *_status)
{
	struct snd_timer_user *tu;
	struct snd_timer_status status;

	tu = file->private_data;
	if (!tu->timeri)
		return -EBADFD;
	memset(&status, 0, sizeof(status));
	status.tstamp = tu->tstamp;
	status.resolution = snd_timer_resolution(tu->timeri);
	status.lost = tu->timeri->lost;
	status.overrun = tu->overrun;
	spin_lock_irq(&tu->qlock);
	status.queue = tu->qused;
	spin_unlock_irq(&tu->qlock);
	if (copy_to_user(_status, &status, sizeof(status)))
		return -EFAULT;
	return 0;
}