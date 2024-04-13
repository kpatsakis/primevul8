int bnxt_re_create_ah(struct ib_ah *ib_ah, struct rdma_ah_attr *ah_attr,
		      u32 flags, struct ib_udata *udata)
{
	struct ib_pd *ib_pd = ib_ah->pd;
	struct bnxt_re_pd *pd = container_of(ib_pd, struct bnxt_re_pd, ib_pd);
	const struct ib_global_route *grh = rdma_ah_read_grh(ah_attr);
	struct bnxt_re_dev *rdev = pd->rdev;
	const struct ib_gid_attr *sgid_attr;
	struct bnxt_re_ah *ah = container_of(ib_ah, struct bnxt_re_ah, ib_ah);
	u8 nw_type;
	int rc;

	if (!(rdma_ah_get_ah_flags(ah_attr) & IB_AH_GRH)) {
		dev_err(rdev_to_dev(rdev), "Failed to alloc AH: GRH not set");
		return -EINVAL;
	}

	ah->rdev = rdev;
	ah->qplib_ah.pd = &pd->qplib_pd;

	/* Supply the configuration for the HW */
	memcpy(ah->qplib_ah.dgid.data, grh->dgid.raw,
	       sizeof(union ib_gid));
	/*
	 * If RoCE V2 is enabled, stack will have two entries for
	 * each GID entry. Avoiding this duplicte entry in HW. Dividing
	 * the GID index by 2 for RoCE V2
	 */
	ah->qplib_ah.sgid_index = grh->sgid_index / 2;
	ah->qplib_ah.host_sgid_index = grh->sgid_index;
	ah->qplib_ah.traffic_class = grh->traffic_class;
	ah->qplib_ah.flow_label = grh->flow_label;
	ah->qplib_ah.hop_limit = grh->hop_limit;
	ah->qplib_ah.sl = rdma_ah_get_sl(ah_attr);

	sgid_attr = grh->sgid_attr;
	/* Get network header type for this GID */
	nw_type = rdma_gid_attr_network_type(sgid_attr);
	ah->qplib_ah.nw_type = bnxt_re_stack_to_dev_nw_type(nw_type);

	memcpy(ah->qplib_ah.dmac, ah_attr->roce.dmac, ETH_ALEN);
	rc = bnxt_qplib_create_ah(&rdev->qplib_res, &ah->qplib_ah,
				  !(flags & RDMA_CREATE_AH_SLEEPABLE));
	if (rc) {
		dev_err(rdev_to_dev(rdev), "Failed to allocate HW AH");
		return rc;
	}

	/* Write AVID to shared page. */
	if (udata) {
		struct bnxt_re_ucontext *uctx = rdma_udata_to_drv_context(
			udata, struct bnxt_re_ucontext, ib_uctx);
		unsigned long flag;
		u32 *wrptr;

		spin_lock_irqsave(&uctx->sh_lock, flag);
		wrptr = (u32 *)(uctx->shpg + BNXT_RE_AVID_OFFT);
		*wrptr = ah->qplib_ah.id;
		wmb(); /* make sure cache is updated. */
		spin_unlock_irqrestore(&uctx->sh_lock, flag);
	}

	return 0;
}