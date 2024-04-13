static void ucma_copy_iw_route(struct rdma_ucm_query_route_resp *resp,
			       struct rdma_route *route)
{
	struct rdma_dev_addr *dev_addr;

	dev_addr = &route->addr.dev_addr;
	rdma_addr_get_dgid(dev_addr, (union ib_gid *) &resp->ib_route[0].dgid);
	rdma_addr_get_sgid(dev_addr, (union ib_gid *) &resp->ib_route[0].sgid);
}