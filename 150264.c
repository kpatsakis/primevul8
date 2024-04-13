static void ucma_query_device_addr(struct rdma_cm_id *cm_id,
				   struct rdma_ucm_query_addr_resp *resp)
{
	if (!cm_id->device)
		return;

	resp->node_guid = (__force __u64) cm_id->device->node_guid;
	resp->ibdev_index = cm_id->device->index;
	resp->port_num = cm_id->port_num;
	resp->pkey = (__force __u16) cpu_to_be16(
		     ib_addr_get_pkey(&cm_id->route.addr.dev_addr));
}