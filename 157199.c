static struct mem_input *dce112_mem_input_create(
	struct dc_context *ctx,
	uint32_t inst)
{
	struct dce_mem_input *dce_mi = kzalloc(sizeof(struct dce_mem_input),
					       GFP_KERNEL);

	if (!dce_mi) {
		BREAK_TO_DEBUGGER();
		return NULL;
	}

	dce112_mem_input_construct(dce_mi, ctx, inst, &mi_regs[inst], &mi_shifts, &mi_masks);
	return &dce_mi->base;
}