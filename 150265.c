static int ucma_get_global_nl_info(struct ib_client_nl_info *res)
{
	res->abi = RDMA_USER_CM_ABI_VERSION;
	res->cdev = ucma_misc.this_device;
	return 0;
}