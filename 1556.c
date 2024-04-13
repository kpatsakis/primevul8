static loff_t nfs4_file_llseek(struct file *filep, loff_t offset, int whence)
{
	loff_t ret;

	switch (whence) {
	case SEEK_HOLE:
	case SEEK_DATA:
		ret = nfs42_proc_llseek(filep, offset, whence);
		if (ret != -EOPNOTSUPP)
			return ret;
		fallthrough;
	default:
		return nfs_file_llseek(filep, offset, whence);
	}
}