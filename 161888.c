static int selinux_file_ioctl(struct file *file, unsigned int cmd,
			      unsigned long arg)
{
	const struct cred *cred = current_cred();
	int error = 0;

	switch (cmd) {
	case FIONREAD:
	/* fall through */
	case FIBMAP:
	/* fall through */
	case FIGETBSZ:
	/* fall through */
	case FS_IOC_GETFLAGS:
	/* fall through */
	case FS_IOC_GETVERSION:
		error = file_has_perm(cred, file, FILE__GETATTR);
		break;

	case FS_IOC_SETFLAGS:
	/* fall through */
	case FS_IOC_SETVERSION:
		error = file_has_perm(cred, file, FILE__SETATTR);
		break;

	/* sys_ioctl() checks */
	case FIONBIO:
	/* fall through */
	case FIOASYNC:
		error = file_has_perm(cred, file, 0);
		break;

	case KDSKBENT:
	case KDSKBSENT:
		error = cred_has_capability(cred, CAP_SYS_TTY_CONFIG,
					    SECURITY_CAP_AUDIT);
		break;

	/* default case assumes that the command will go
	 * to the file's ioctl() function.
	 */
	default:
		error = file_has_perm(cred, file, FILE__IOCTL);
	}
	return error;
}