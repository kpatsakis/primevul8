static void nfs_flush_error(struct nfs_pageio_descriptor *desc,
		struct nfs_pgio_header *hdr)
{
	set_bit(NFS_IOHDR_REDO, &hdr->flags);
	while (!list_empty(&hdr->rpc_list)) {
		struct nfs_write_data *data = list_first_entry(&hdr->rpc_list,
				struct nfs_write_data, list);
		list_del(&data->list);
		nfs_writedata_release(data);
	}
	desc->pg_completion_ops->error_cleanup(&desc->pg_list);
}