static bool dce110_validate_surface_sets(
		struct dc_state *context)
{
	int i, j;

	for (i = 0; i < context->stream_count; i++) {
		if (context->stream_status[i].plane_count == 0)
			continue;

		if (context->stream_status[i].plane_count > 2)
			return false;

		for (j = 0; j < context->stream_status[i].plane_count; j++) {
			struct dc_plane_state *plane =
				context->stream_status[i].plane_states[j];

			/* underlay validation */
			if (plane->format >= SURFACE_PIXEL_FORMAT_VIDEO_BEGIN) {

				if ((plane->src_rect.width > 1920 ||
					plane->src_rect.height > 1080))
					return false;

				/* we don't have the logic to support underlay
				 * only yet so block the use case where we get
				 * NV12 plane as top layer
				 */
				if (j == 0)
					return false;

				/* irrespective of plane format,
				 * stream should be RGB encoded
				 */
				if (context->streams[i]->timing.pixel_encoding
						!= PIXEL_ENCODING_RGB)
					return false;

			}

		}
	}

	return true;
}