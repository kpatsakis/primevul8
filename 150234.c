static void ucma_copy_ud_event(struct ib_device *device,
			       struct rdma_ucm_ud_param *dst,
			       struct rdma_ud_param *src)
{
	if (src->private_data_len)
		memcpy(dst->private_data, src->private_data,
		       src->private_data_len);
	dst->private_data_len = src->private_data_len;
	ib_copy_ah_attr_to_user(device, &dst->ah_attr, &src->ah_attr);
	dst->qp_num = src->qp_num;
	dst->qkey = src->qkey;
}