static int snd_timer_user_fasync(int fd, struct file * file, int on)
{
	struct snd_timer_user *tu;

	tu = file->private_data;
	return fasync_helper(fd, file, on, &tu->fasync);
}