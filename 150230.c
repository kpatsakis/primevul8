static void ucma_copy_ib_route(struct rdma_ucm_query_route_resp *resp,
			       struct rdma_route *route)
{
	struct rdma_dev_addr *dev_addr;

	resp->num_paths = route->num_paths;
	switch (route->num_paths) {
	case 0:
		dev_addr = &route->addr.dev_addr;
		rdma_addr_get_dgid(dev_addr,
				   (union ib_gid *) &resp->ib_route[0].dgid);
		rdma_addr_get_sgid(dev_addr,
				   (union ib_gid *) &resp->ib_route[0].sgid);
		resp->ib_route[0].pkey = cpu_to_be16(ib_addr_get_pkey(dev_addr));
		break;
	case 2:
		ib_copy_path_rec_to_user(&resp->ib_route[1],
					 &route->path_rec[1]);
		fallthrough;
	case 1:
		ib_copy_path_rec_to_user(&resp->ib_route[0],
					 &route->path_rec[0]);
		break;
	default:
		break;
	}
}