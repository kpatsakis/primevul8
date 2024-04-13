static struct file *__nfs42_ssc_open(struct vfsmount *ss_mnt,
		struct nfs_fh *src_fh, nfs4_stateid *stateid)
{
	struct nfs_fattr *fattr = nfs_alloc_fattr();
	struct file *filep, *res;
	struct nfs_server *server;
	struct inode *r_ino = NULL;
	struct nfs_open_context *ctx;
	struct nfs4_state_owner *sp;
	char *read_name = NULL;
	int len, status = 0;

	server = NFS_SERVER(ss_mnt->mnt_root->d_inode);

	if (!fattr)
		return ERR_PTR(-ENOMEM);

	status = nfs4_proc_getattr(server, src_fh, fattr, NULL);
	if (status < 0) {
		res = ERR_PTR(status);
		goto out;
	}

	res = ERR_PTR(-ENOMEM);
	len = strlen(SSC_READ_NAME_BODY) + 16;
	read_name = kzalloc(len, GFP_KERNEL);
	if (read_name == NULL)
		goto out;
	snprintf(read_name, len, SSC_READ_NAME_BODY, read_name_gen++);

	r_ino = nfs_fhget(ss_mnt->mnt_root->d_inode->i_sb, src_fh, fattr);
	if (IS_ERR(r_ino)) {
		res = ERR_CAST(r_ino);
		goto out_free_name;
	}

	filep = alloc_file_pseudo(r_ino, ss_mnt, read_name, O_RDONLY,
				     r_ino->i_fop);
	if (IS_ERR(filep)) {
		res = ERR_CAST(filep);
		goto out_free_name;
	}

	ctx = alloc_nfs_open_context(filep->f_path.dentry, filep->f_mode,
					filep);
	if (IS_ERR(ctx)) {
		res = ERR_CAST(ctx);
		goto out_filep;
	}

	res = ERR_PTR(-EINVAL);
	sp = nfs4_get_state_owner(server, ctx->cred, GFP_KERNEL);
	if (sp == NULL)
		goto out_ctx;

	ctx->state = nfs4_get_open_state(r_ino, sp);
	if (ctx->state == NULL)
		goto out_stateowner;

	set_bit(NFS_SRV_SSC_COPY_STATE, &ctx->state->flags);
	memcpy(&ctx->state->open_stateid.other, &stateid->other,
	       NFS4_STATEID_OTHER_SIZE);
	update_open_stateid(ctx->state, stateid, NULL, filep->f_mode);
	set_bit(NFS_OPEN_STATE, &ctx->state->flags);

	nfs_file_set_open_context(filep, ctx);
	put_nfs_open_context(ctx);

	file_ra_state_init(&filep->f_ra, filep->f_mapping->host->i_mapping);
	res = filep;
out_free_name:
	kfree(read_name);
out:
	nfs_free_fattr(fattr);
	return res;
out_stateowner:
	nfs4_put_state_owner(sp);
out_ctx:
	put_nfs_open_context(ctx);
out_filep:
	fput(filep);
	goto out_free_name;
}