static void dcn10_destroy_resource_pool(struct resource_pool **pool)
{
	struct dcn10_resource_pool *dcn10_pool = TO_DCN10_RES_POOL(*pool);

	destruct(dcn10_pool);
	kfree(dcn10_pool);
	*pool = NULL;
}