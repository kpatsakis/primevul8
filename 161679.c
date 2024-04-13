SYSCALL_DEFINE2(nanosleep, struct timespec __user *, rqtp,
		struct timespec __user *, rmtp)
{
	struct timespec tu;

	if (copy_from_user(&tu, rqtp, sizeof(tu)))
		return -EFAULT;

	if (!timespec_valid(&tu))
		return -EINVAL;

	return hrtimer_nanosleep(&tu, rmtp, HRTIMER_MODE_REL, CLOCK_MONOTONIC);
}