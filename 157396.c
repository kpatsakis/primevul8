static struct bnxt_re_ah *bnxt_re_create_shadow_qp_ah
				(struct bnxt_re_pd *pd,
				 struct bnxt_qplib_res *qp1_res,
				 struct bnxt_qplib_qp *qp1_qp)
{
	struct bnxt_re_dev *rdev = pd->rdev;
	struct bnxt_re_ah *ah;
	union ib_gid sgid;
	int rc;

	ah = kzalloc(sizeof(*ah), GFP_KERNEL);
	if (!ah)
		return NULL;

	ah->rdev = rdev;
	ah->qplib_ah.pd = &pd->qplib_pd;

	rc = bnxt_re_query_gid(&rdev->ibdev, 1, 0, &sgid);
	if (rc)
		goto fail;

	/* supply the dgid data same as sgid */
	memcpy(ah->qplib_ah.dgid.data, &sgid.raw,
	       sizeof(union ib_gid));
	ah->qplib_ah.sgid_index = 0;

	ah->qplib_ah.traffic_class = 0;
	ah->qplib_ah.flow_label = 0;
	ah->qplib_ah.hop_limit = 1;
	ah->qplib_ah.sl = 0;
	/* Have DMAC same as SMAC */
	ether_addr_copy(ah->qplib_ah.dmac, rdev->netdev->dev_addr);

	rc = bnxt_qplib_create_ah(&rdev->qplib_res, &ah->qplib_ah, false);
	if (rc) {
		dev_err(rdev_to_dev(rdev),
			"Failed to allocate HW AH for Shadow QP");
		goto fail;
	}

	return ah;

fail:
	kfree(ah);
	return NULL;
}