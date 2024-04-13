int bnxt_re_del_gid(const struct ib_gid_attr *attr, void **context)
{
	int rc = 0;
	struct bnxt_re_gid_ctx *ctx, **ctx_tbl;
	struct bnxt_re_dev *rdev = to_bnxt_re_dev(attr->device, ibdev);
	struct bnxt_qplib_sgid_tbl *sgid_tbl = &rdev->qplib_res.sgid_tbl;
	struct bnxt_qplib_gid *gid_to_del;
	u16 vlan_id = 0xFFFF;

	/* Delete the entry from the hardware */
	ctx = *context;
	if (!ctx)
		return -EINVAL;

	if (sgid_tbl && sgid_tbl->active) {
		if (ctx->idx >= sgid_tbl->max)
			return -EINVAL;
		gid_to_del = &sgid_tbl->tbl[ctx->idx].gid;
		vlan_id = sgid_tbl->tbl[ctx->idx].vlan_id;
		/* DEL_GID is called in WQ context(netdevice_event_work_handler)
		 * or via the ib_unregister_device path. In the former case QP1
		 * may not be destroyed yet, in which case just return as FW
		 * needs that entry to be present and will fail it's deletion.
		 * We could get invoked again after QP1 is destroyed OR get an
		 * ADD_GID call with a different GID value for the same index
		 * where we issue MODIFY_GID cmd to update the GID entry -- TBD
		 */
		if (ctx->idx == 0 &&
		    rdma_link_local_addr((struct in6_addr *)gid_to_del) &&
		    ctx->refcnt == 1 && rdev->qp1_sqp) {
			dev_dbg(rdev_to_dev(rdev),
				"Trying to delete GID0 while QP1 is alive\n");
			return -EFAULT;
		}
		ctx->refcnt--;
		if (!ctx->refcnt) {
			rc = bnxt_qplib_del_sgid(sgid_tbl, gid_to_del,
						 vlan_id,  true);
			if (rc) {
				dev_err(rdev_to_dev(rdev),
					"Failed to remove GID: %#x", rc);
			} else {
				ctx_tbl = sgid_tbl->ctx;
				ctx_tbl[ctx->idx] = NULL;
				kfree(ctx);
			}
		}
	} else {
		return -EINVAL;
	}
	return rc;
}