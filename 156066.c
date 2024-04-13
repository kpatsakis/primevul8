int nfs_write_need_commit(struct nfs_write_data *data)
{
	if (data->verf.committed == NFS_DATA_SYNC)
		return data->header->lseg == NULL;
	return data->verf.committed != NFS_FILE_SYNC;
}