static void read_dce_straps(
	struct dc_context *ctx,
	struct resource_straps *straps)
{
	uint32_t reg_val = dm_read_reg_soc15(ctx, mmCC_DC_MISC_STRAPS, 0);

	straps->audio_stream_number = get_reg_field_value(reg_val,
							  CC_DC_MISC_STRAPS,
							  AUDIO_STREAM_NUMBER);
	straps->hdmi_disable = get_reg_field_value(reg_val,
						   CC_DC_MISC_STRAPS,
						   HDMI_DISABLE);

	reg_val = dm_read_reg_soc15(ctx, mmDC_PINSTRAPS, 0);
	straps->dc_pinstraps_audio = get_reg_field_value(reg_val,
							 DC_PINSTRAPS,
							 DC_PINSTRAPS_AUDIO);
}