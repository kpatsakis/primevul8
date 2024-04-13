static enum dc_status dcn10_validate_global(struct dc *dc, struct dc_state *context)
{
	int i, j;
	bool video_down_scaled = false;
	bool video_large = false;
	bool desktop_large = false;
	bool dcc_disabled = false;

	for (i = 0; i < context->stream_count; i++) {
		if (context->stream_status[i].plane_count == 0)
			continue;

		if (context->stream_status[i].plane_count > 2)
			return DC_FAIL_UNSUPPORTED_1;

		for (j = 0; j < context->stream_status[i].plane_count; j++) {
			struct dc_plane_state *plane =
				context->stream_status[i].plane_states[j];


			if (plane->format >= SURFACE_PIXEL_FORMAT_VIDEO_BEGIN) {

				if (plane->src_rect.width > plane->dst_rect.width ||
						plane->src_rect.height > plane->dst_rect.height)
					video_down_scaled = true;

				if (plane->src_rect.width >= 3840)
					video_large = true;

			} else {
				if (plane->src_rect.width >= 3840)
					desktop_large = true;
				if (!plane->dcc.enable)
					dcc_disabled = true;
			}
		}
	}

	/*
	 * Workaround: On DCN10 there is UMC issue that causes underflow when
	 * playing 4k video on 4k desktop with video downscaled and single channel
	 * memory
	 */
	if (video_large && desktop_large && video_down_scaled && dcc_disabled &&
			dc->dcn_soc->number_of_channels == 1)
		return DC_FAIL_SURFACE_VALIDATE;

	return DC_OK;
}