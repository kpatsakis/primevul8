static struct pipe_ctx *dce110_acquire_underlay(
		struct dc_state *context,
		const struct resource_pool *pool,
		struct dc_stream_state *stream)
{
	struct dc *dc = stream->ctx->dc;
	struct resource_context *res_ctx = &context->res_ctx;
	unsigned int underlay_idx = pool->underlay_pipe_index;
	struct pipe_ctx *pipe_ctx = &res_ctx->pipe_ctx[underlay_idx];

	if (res_ctx->pipe_ctx[underlay_idx].stream)
		return NULL;

	pipe_ctx->stream_res.tg = pool->timing_generators[underlay_idx];
	pipe_ctx->plane_res.mi = pool->mis[underlay_idx];
	/*pipe_ctx->plane_res.ipp = res_ctx->pool->ipps[underlay_idx];*/
	pipe_ctx->plane_res.xfm = pool->transforms[underlay_idx];
	pipe_ctx->stream_res.opp = pool->opps[underlay_idx];
	pipe_ctx->pipe_idx = underlay_idx;

	pipe_ctx->stream = stream;

	if (!dc->current_state->res_ctx.pipe_ctx[underlay_idx].stream) {
		struct tg_color black_color = {0};
		struct dc_bios *dcb = dc->ctx->dc_bios;

		dc->hwss.enable_display_power_gating(
				dc,
				pipe_ctx->stream_res.tg->inst,
				dcb, PIPE_GATING_CONTROL_DISABLE);

		/*
		 * This is for powering on underlay, so crtc does not
		 * need to be enabled
		 */

		pipe_ctx->stream_res.tg->funcs->program_timing(pipe_ctx->stream_res.tg,
				&stream->timing,
				0,
				0,
				0,
				0,
				pipe_ctx->stream->signal,
				false);

		pipe_ctx->stream_res.tg->funcs->enable_advanced_request(
				pipe_ctx->stream_res.tg,
				true,
				&stream->timing);

		pipe_ctx->plane_res.mi->funcs->allocate_mem_input(pipe_ctx->plane_res.mi,
				stream->timing.h_total,
				stream->timing.v_total,
				stream->timing.pix_clk_100hz / 10,
				context->stream_count);

		color_space_to_black_color(dc,
				COLOR_SPACE_YCBCR601, &black_color);
		pipe_ctx->stream_res.tg->funcs->set_blank_color(
				pipe_ctx->stream_res.tg,
				&black_color);
	}

	return pipe_ctx;
}