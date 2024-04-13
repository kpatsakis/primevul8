static void read_dce_straps(
	struct dc_context *ctx,
	struct resource_straps *straps)
{
	generic_reg_get(ctx, mmDC_PINSTRAPS + BASE(mmDC_PINSTRAPS_BASE_IDX),
		FN(DC_PINSTRAPS, DC_PINSTRAPS_AUDIO), &straps->dc_pinstraps_audio);
}