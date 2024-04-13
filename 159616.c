static int snd_timer_user_pause(struct file *file)
{
	int err;
	struct snd_timer_user *tu;

	tu = file->private_data;
	if (!tu->timeri)
		return -EBADFD;
	return (err = snd_timer_pause(tu->timeri)) < 0 ? err : 0;
}