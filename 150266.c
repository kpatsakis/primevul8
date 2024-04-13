static void ucma_copy_iboe_route(struct rdma_ucm_query_route_resp *resp,
				 struct rdma_route *route)
{

	resp->num_paths = route->num_paths;
	switch (route->num_paths) {
	case 0:
		rdma_ip2gid((struct sockaddr *)&route->addr.dst_addr,
			    (union ib_gid *)&resp->ib_route[0].dgid);
		rdma_ip2gid((struct sockaddr *)&route->addr.src_addr,
			    (union ib_gid *)&resp->ib_route[0].sgid);
		resp->ib_route[0].pkey = cpu_to_be16(0xffff);
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