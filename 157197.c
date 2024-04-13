static struct mpc *dcn10_mpc_create(struct dc_context *ctx)
{
	struct dcn10_mpc *mpc10 = kzalloc(sizeof(struct dcn10_mpc),
					  GFP_KERNEL);

	if (!mpc10)
		return NULL;

	dcn10_mpc_construct(mpc10, ctx,
			&mpc_regs,
			&mpc_shift,
			&mpc_mask,
			4);

	return &mpc10->base;
}