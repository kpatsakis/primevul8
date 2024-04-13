static void dcn10_dpp_destroy(struct dpp **dpp)
{
	kfree(TO_DCN10_DPP(*dpp));
	*dpp = NULL;
}