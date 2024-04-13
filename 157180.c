static void get_pixel_clock_parameters(
	const struct pipe_ctx *pipe_ctx,
	struct pixel_clk_params *pixel_clk_params)
{
	const struct dc_stream_state *stream = pipe_ctx->stream;
	pixel_clk_params->requested_pix_clk_100hz = stream->timing.pix_clk_100hz;
	pixel_clk_params->encoder_object_id = stream->link->link_enc->id;
	pixel_clk_params->signal_type = pipe_ctx->stream->signal;
	pixel_clk_params->controller_id = pipe_ctx->stream_res.tg->inst + 1;
	/* TODO: un-hardcode*/
	pixel_clk_params->requested_sym_clk = LINK_RATE_LOW *
		LINK_RATE_REF_FREQ_IN_KHZ;
	pixel_clk_params->flags.ENABLE_SS = 0;
	pixel_clk_params->color_depth =
		stream->timing.display_color_depth;
	pixel_clk_params->flags.DISPLAY_BLANKED = 1;
	pixel_clk_params->pixel_encoding = stream->timing.pixel_encoding;

	if (stream->timing.pixel_encoding == PIXEL_ENCODING_YCBCR422)
		pixel_clk_params->color_depth = COLOR_DEPTH_888;

	if (stream->timing.pixel_encoding == PIXEL_ENCODING_YCBCR420)
		pixel_clk_params->requested_pix_clk_100hz  /= 2;
	if (stream->timing.timing_3d_format == TIMING_3D_FORMAT_HW_FRAME_PACKING)
		pixel_clk_params->requested_pix_clk_100hz *= 2;

}