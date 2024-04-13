static ssize_t nfs4_copy_file_range(struct file *file_in, loff_t pos_in,
				    struct file *file_out, loff_t pos_out,
				    size_t count, unsigned int flags)
{
	ssize_t ret;

	ret = __nfs4_copy_file_range(file_in, pos_in, file_out, pos_out, count,
				     flags);
	if (ret == -EOPNOTSUPP || ret == -EXDEV)
		ret = generic_copy_file_range(file_in, pos_in, file_out,
					      pos_out, count, flags);
	return ret;
}