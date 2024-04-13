struct stream_encoder *dce100_find_first_free_match_stream_enc_for_link(
		struct resource_context *res_ctx,
		const struct resource_pool *pool,
		struct dc_stream_state *stream)
{
	int i;
	int j = -1;
	struct dc_link *link = stream->link;

	for (i = 0; i < pool->stream_enc_count; i++) {
		if (!res_ctx->is_stream_enc_acquired[i] &&
				pool->stream_enc[i]) {
			/* Store first available for MST second display
			 * in daisy chain use case
			 */
			j = i;
			if (pool->stream_enc[i]->id ==
					link->link_enc->preferred_engine)
				return pool->stream_enc[i];
		}
	}

	/*
	 * below can happen in cases when stream encoder is acquired:
	 * 1) for second MST display in chain, so preferred engine already
	 * acquired;
	 * 2) for another link, which preferred engine already acquired by any
	 * MST configuration.
	 *
	 * If signal is of DP type and preferred engine not found, return last available
	 *
	 * TODO - This is just a patch up and a generic solution is
	 * required for non DP connectors.
	 */

	if (j >= 0 && link->connector_signal == SIGNAL_TYPE_DISPLAY_PORT)
		return pool->stream_enc[j];

	return NULL;
}