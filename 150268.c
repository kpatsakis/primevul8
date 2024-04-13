static void ucma_copy_conn_param(struct rdma_cm_id *id,
				 struct rdma_conn_param *dst,
				 struct rdma_ucm_conn_param *src)
{
	dst->private_data = src->private_data;
	dst->private_data_len = src->private_data_len;
	dst->responder_resources =src->responder_resources;
	dst->initiator_depth = src->initiator_depth;
	dst->flow_control = src->flow_control;
	dst->retry_count = src->retry_count;
	dst->rnr_retry_count = src->rnr_retry_count;
	dst->srq = src->srq;
	dst->qp_num = src->qp_num & 0xFFFFFF;
	dst->qkey = (id->route.addr.src_addr.ss_family == AF_IB) ? src->qkey : 0;
}