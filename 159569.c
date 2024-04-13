static int snd_timer_user_continue(struct file *file)
{
	int err;
	struct snd_timer_user *tu;

	tu = file->private_data;
	if (!tu->timeri)
		return -EBADFD;
	tu->timeri->lost = 0;
	return (err = snd_timer_continue(tu->timeri)) < 0 ? err : 0;
}