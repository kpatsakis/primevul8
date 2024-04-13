void nfs_write_prepare(struct rpc_task *task, void *calldata)
{
	struct nfs_write_data *data = calldata;
	NFS_PROTO(data->header->inode)->write_rpc_prepare(task, data);
	if (unlikely(test_bit(NFS_CONTEXT_BAD, &data->args.context->flags)))
		rpc_exit(task, -EIO);
}