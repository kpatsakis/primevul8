static ssize_t ucma_query_path(struct ucma_context *ctx,
			       void __user *response, int out_len)
{
	struct rdma_ucm_query_path_resp *resp;
	int i, ret = 0;

	if (out_len < sizeof(*resp))
		return -ENOSPC;

	resp = kzalloc(out_len, GFP_KERNEL);
	if (!resp)
		return -ENOMEM;

	resp->num_paths = ctx->cm_id->route.num_paths;
	for (i = 0, out_len -= sizeof(*resp);
	     i < resp->num_paths && out_len > sizeof(struct ib_path_rec_data);
	     i++, out_len -= sizeof(struct ib_path_rec_data)) {
		struct sa_path_rec *rec = &ctx->cm_id->route.path_rec[i];

		resp->path_data[i].flags = IB_PATH_GMP | IB_PATH_PRIMARY |
					   IB_PATH_BIDIRECTIONAL;
		if (rec->rec_type == SA_PATH_REC_TYPE_OPA) {
			struct sa_path_rec ib;

			sa_convert_path_opa_to_ib(&ib, rec);
			ib_sa_pack_path(&ib, &resp->path_data[i].path_rec);

		} else {
			ib_sa_pack_path(rec, &resp->path_data[i].path_rec);
		}
	}

	if (copy_to_user(response, resp, struct_size(resp, path_data, i)))
		ret = -EFAULT;

	kfree(resp);
	return ret;
}