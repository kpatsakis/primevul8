static uint32_t read_pipe_fuses(struct dc_context *ctx)
{
	uint32_t value = dm_read_reg_soc15(ctx, mmCC_DC_PIPE_DIS, 0);
	/* VG20 support max 6 pipes */
	value = value & 0x3f;
	return value;
}