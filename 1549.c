static ssize_t __nfs4_copy_file_range(struct file *file_in, loff_t pos_in,
				      struct file *file_out, loff_t pos_out,
				      size_t count, unsigned int flags)
{
	struct nfs42_copy_notify_res *cn_resp = NULL;
	struct nl4_server *nss = NULL;
	nfs4_stateid *cnrs = NULL;
	ssize_t ret;
	bool sync = false;

	/* Only offload copy if superblock is the same */
	if (file_in->f_op != &nfs4_file_operations)
		return -EXDEV;
	if (!nfs_server_capable(file_inode(file_out), NFS_CAP_COPY) ||
	    !nfs_server_capable(file_inode(file_in), NFS_CAP_COPY))
		return -EOPNOTSUPP;
	if (file_inode(file_in) == file_inode(file_out))
		return -EOPNOTSUPP;
	/* if the copy size if smaller than 2 RPC payloads, make it
	 * synchronous
	 */
	if (count <= 2 * NFS_SERVER(file_inode(file_in))->rsize)
		sync = true;
retry:
	if (!nfs42_files_from_same_server(file_in, file_out)) {
		/*
		 * for inter copy, if copy size is too small
		 * then fallback to generic copy.
		 */
		if (sync)
			return -EOPNOTSUPP;
		cn_resp = kzalloc(sizeof(struct nfs42_copy_notify_res),
				  GFP_KERNEL);
		if (unlikely(cn_resp == NULL))
			return -ENOMEM;

		ret = nfs42_proc_copy_notify(file_in, file_out, cn_resp);
		if (ret) {
			ret = -EOPNOTSUPP;
			goto out;
		}
		nss = &cn_resp->cnr_src;
		cnrs = &cn_resp->cnr_stateid;
	}
	ret = nfs42_proc_copy(file_in, pos_in, file_out, pos_out, count,
				nss, cnrs, sync);
out:
	kfree(cn_resp);

	if (ret == -EAGAIN)
		goto retry;
	return ret;
}