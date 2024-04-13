sg_proc_write_adio(struct file *filp, const char __user *buffer,
		   size_t count, loff_t *off)
{
	int err;
	unsigned long num;

	if (!capable(CAP_SYS_ADMIN) || !capable(CAP_SYS_RAWIO))
		return -EACCES;
	err = kstrtoul_from_user(buffer, count, 0, &num);
	if (err)
		return err;
	sg_allow_dio = num ? 1 : 0;
	return count;
}