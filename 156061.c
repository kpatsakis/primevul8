void nfs_init_commit(struct nfs_commit_data *data,
		     struct list_head *head,
		     struct pnfs_layout_segment *lseg,
		     struct nfs_commit_info *cinfo)
{
	struct nfs_page *first = nfs_list_entry(head->next);
	struct inode *inode = first->wb_context->dentry->d_inode;

	/* Set up the RPC argument and reply structs
	 * NB: take care not to mess about with data->commit et al. */

	list_splice_init(head, &data->pages);

	data->inode	  = inode;
	data->cred	  = first->wb_context->cred;
	data->lseg	  = lseg; /* reference transferred */
	data->mds_ops     = &nfs_commit_ops;
	data->completion_ops = cinfo->completion_ops;
	data->dreq	  = cinfo->dreq;

	data->args.fh     = NFS_FH(data->inode);
	/* Note: we always request a commit of the entire inode */
	data->args.offset = 0;
	data->args.count  = 0;
	data->context     = get_nfs_open_context(first->wb_context);
	data->res.fattr   = &data->fattr;
	data->res.verf    = &data->verf;
	nfs_fattr_init(&data->fattr);
}