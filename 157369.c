int bnxt_re_add_gid(const struct ib_gid_attr *attr, void **context)
{
	int rc;
	u32 tbl_idx = 0;
	u16 vlan_id = 0xFFFF;
	struct bnxt_re_gid_ctx *ctx, **ctx_tbl;
	struct bnxt_re_dev *rdev = to_bnxt_re_dev(attr->device, ibdev);
	struct bnxt_qplib_sgid_tbl *sgid_tbl = &rdev->qplib_res.sgid_tbl;

	rc = rdma_read_gid_l2_fields(attr, &vlan_id, NULL);
	if (rc)
		return rc;

	rc = bnxt_qplib_add_sgid(sgid_tbl, (struct bnxt_qplib_gid *)&attr->gid,
				 rdev->qplib_res.netdev->dev_addr,
				 vlan_id, true, &tbl_idx);
	if (rc == -EALREADY) {
		ctx_tbl = sgid_tbl->ctx;
		ctx_tbl[tbl_idx]->refcnt++;
		*context = ctx_tbl[tbl_idx];
		return 0;
	}

	if (rc < 0) {
		dev_err(rdev_to_dev(rdev), "Failed to add GID: %#x", rc);
		return rc;
	}

	ctx = kmalloc(sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;
	ctx_tbl = sgid_tbl->ctx;
	ctx->idx = tbl_idx;
	ctx->refcnt = 1;
	ctx_tbl[tbl_idx] = ctx;
	*context = ctx;

	return rc;
}