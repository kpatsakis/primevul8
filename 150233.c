static ssize_t ucma_join_ip_multicast(struct ucma_file *file,
				      const char __user *inbuf,
				      int in_len, int out_len)
{
	struct rdma_ucm_join_ip_mcast cmd;
	struct rdma_ucm_join_mcast join_cmd;

	if (copy_from_user(&cmd, inbuf, sizeof(cmd)))
		return -EFAULT;

	join_cmd.response = cmd.response;
	join_cmd.uid = cmd.uid;
	join_cmd.id = cmd.id;
	join_cmd.addr_size = rdma_addr_size_in6(&cmd.addr);
	if (!join_cmd.addr_size)
		return -EINVAL;

	join_cmd.join_flags = RDMA_MC_JOIN_FLAG_FULLMEMBER;
	memcpy(&join_cmd.addr, &cmd.addr, join_cmd.addr_size);

	return ucma_process_join(file, &join_cmd, out_len);
}