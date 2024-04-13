static struct clock_source *find_matching_pll(
		struct resource_context *res_ctx,
		const struct resource_pool *pool,
		const struct dc_stream_state *const stream)
{
	switch (stream->link->link_enc->transmitter) {
	case TRANSMITTER_UNIPHY_A:
		return pool->clock_sources[DCE112_CLK_SRC_PLL0];
	case TRANSMITTER_UNIPHY_B:
		return pool->clock_sources[DCE112_CLK_SRC_PLL1];
	case TRANSMITTER_UNIPHY_C:
		return pool->clock_sources[DCE112_CLK_SRC_PLL2];
	case TRANSMITTER_UNIPHY_D:
		return pool->clock_sources[DCE112_CLK_SRC_PLL3];
	case TRANSMITTER_UNIPHY_E:
		return pool->clock_sources[DCE112_CLK_SRC_PLL4];
	case TRANSMITTER_UNIPHY_F:
		return pool->clock_sources[DCE112_CLK_SRC_PLL5];
	default:
		return NULL;
	};

	return 0;
}