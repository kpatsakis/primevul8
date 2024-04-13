static uint32_t read_pipe_fuses(struct dc_context *ctx)
{
	uint32_t value = dm_read_reg_soc15(ctx, mmCC_DC_PIPE_DIS, 0);
	/* RV1 support max 4 pipes */
	value = value & 0xf;
	return value;
}