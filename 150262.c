static ssize_t ucma_write(struct file *filp, const char __user *buf,
			  size_t len, loff_t *pos)
{
	struct ucma_file *file = filp->private_data;
	struct rdma_ucm_cmd_hdr hdr;
	ssize_t ret;

	if (!ib_safe_file_access(filp)) {
		pr_err_once("ucma_write: process %d (%s) changed security contexts after opening file descriptor, this is not allowed.\n",
			    task_tgid_vnr(current), current->comm);
		return -EACCES;
	}

	if (len < sizeof(hdr))
		return -EINVAL;

	if (copy_from_user(&hdr, buf, sizeof(hdr)))
		return -EFAULT;

	if (hdr.cmd >= ARRAY_SIZE(ucma_cmd_table))
		return -EINVAL;
	hdr.cmd = array_index_nospec(hdr.cmd, ARRAY_SIZE(ucma_cmd_table));

	if (hdr.in + sizeof(hdr) > len)
		return -EINVAL;

	if (!ucma_cmd_table[hdr.cmd])
		return -ENOSYS;

	ret = ucma_cmd_table[hdr.cmd](file, buf + sizeof(hdr), hdr.in, hdr.out);
	if (!ret)
		ret = len;

	return ret;
}