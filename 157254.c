static struct transform *dce120_transform_create(
	struct dc_context *ctx,
	uint32_t inst)
{
	struct dce_transform *transform =
		kzalloc(sizeof(struct dce_transform), GFP_KERNEL);

	if (!transform)
		return NULL;

	dce_transform_construct(transform, ctx, inst,
				&xfm_regs[inst], &xfm_shift, &xfm_mask);
	transform->lb_memory_size = 0x1404; /*5124*/
	return &transform->base;
}