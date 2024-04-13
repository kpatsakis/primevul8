static bool underlay_create(struct dc_context *ctx, struct resource_pool *pool)
{
	struct dce110_timing_generator *dce110_tgv = kzalloc(sizeof(*dce110_tgv),
							     GFP_KERNEL);
	struct dce_transform *dce110_xfmv = kzalloc(sizeof(*dce110_xfmv),
						    GFP_KERNEL);
	struct dce_mem_input *dce110_miv = kzalloc(sizeof(*dce110_miv),
						   GFP_KERNEL);
	struct dce110_opp *dce110_oppv = kzalloc(sizeof(*dce110_oppv),
						 GFP_KERNEL);

	if (!dce110_tgv || !dce110_xfmv || !dce110_miv || !dce110_oppv) {
		kfree(dce110_tgv);
		kfree(dce110_xfmv);
		kfree(dce110_miv);
		kfree(dce110_oppv);
		return false;
	}

	dce110_opp_v_construct(dce110_oppv, ctx);

	dce110_timing_generator_v_construct(dce110_tgv, ctx);
	dce110_mem_input_v_construct(dce110_miv, ctx);
	dce110_transform_v_construct(dce110_xfmv, ctx);

	pool->opps[pool->pipe_count] = &dce110_oppv->base;
	pool->timing_generators[pool->pipe_count] = &dce110_tgv->base;
	pool->mis[pool->pipe_count] = &dce110_miv->base;
	pool->transforms[pool->pipe_count] = &dce110_xfmv->base;
	pool->pipe_count++;

	/* update the public caps to indicate an underlay is available */
	ctx->dc->caps.max_slave_planes = 1;
	ctx->dc->caps.max_slave_planes = 1;

	return true;
}