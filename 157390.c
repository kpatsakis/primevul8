int bnxt_re_query_ah(struct ib_ah *ib_ah, struct rdma_ah_attr *ah_attr)
{
	struct bnxt_re_ah *ah = container_of(ib_ah, struct bnxt_re_ah, ib_ah);

	ah_attr->type = ib_ah->type;
	rdma_ah_set_sl(ah_attr, ah->qplib_ah.sl);
	memcpy(ah_attr->roce.dmac, ah->qplib_ah.dmac, ETH_ALEN);
	rdma_ah_set_grh(ah_attr, NULL, 0,
			ah->qplib_ah.host_sgid_index,
			0, ah->qplib_ah.traffic_class);
	rdma_ah_set_dgid_raw(ah_attr, ah->qplib_ah.dgid.data);
	rdma_ah_set_port_num(ah_attr, 1);
	rdma_ah_set_static_rate(ah_attr, 0);
	return 0;
}