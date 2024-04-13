
static int io_get_ext_arg(unsigned flags, const void __user *argp, size_t *argsz,
			  struct __kernel_timespec __user **ts,
			  const sigset_t __user **sig)
{
	struct io_uring_getevents_arg arg;

	/*
	 * If EXT_ARG isn't set, then we have no timespec and the argp pointer
	 * is just a pointer to the sigset_t.
	 */
	if (!(flags & IORING_ENTER_EXT_ARG)) {
		*sig = (const sigset_t __user *) argp;
		*ts = NULL;
		return 0;
	}

	/*
	 * EXT_ARG is set - ensure we agree on the size of it and copy in our
	 * timespec and sigset_t pointers if good.
	 */
	if (*argsz != sizeof(arg))
		return -EINVAL;
	if (copy_from_user(&arg, argp, sizeof(arg)))
		return -EFAULT;
	if (arg.pad)
		return -EINVAL;
	*sig = u64_to_user_ptr(arg.sigmask);
	*argsz = arg.sigmask_sz;
	*ts = u64_to_user_ptr(arg.ts);
	return 0;