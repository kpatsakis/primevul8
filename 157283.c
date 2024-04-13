static void bw_calcs_data_update_from_pplib(struct dc *dc)
{
	struct dm_pp_clock_levels clks = {0};

	/*do system clock*/
	dm_pp_get_clock_levels_by_type(
			dc->ctx,
			DM_PP_CLOCK_TYPE_ENGINE_CLK,
			&clks);
	/* convert all the clock fro kHz to fix point mHz */
	dc->bw_vbios->high_sclk = bw_frc_to_fixed(
			clks.clocks_in_khz[clks.num_levels-1], 1000);
	dc->bw_vbios->mid1_sclk  = bw_frc_to_fixed(
			clks.clocks_in_khz[clks.num_levels/8], 1000);
	dc->bw_vbios->mid2_sclk  = bw_frc_to_fixed(
			clks.clocks_in_khz[clks.num_levels*2/8], 1000);
	dc->bw_vbios->mid3_sclk  = bw_frc_to_fixed(
			clks.clocks_in_khz[clks.num_levels*3/8], 1000);
	dc->bw_vbios->mid4_sclk  = bw_frc_to_fixed(
			clks.clocks_in_khz[clks.num_levels*4/8], 1000);
	dc->bw_vbios->mid5_sclk  = bw_frc_to_fixed(
			clks.clocks_in_khz[clks.num_levels*5/8], 1000);
	dc->bw_vbios->mid6_sclk  = bw_frc_to_fixed(
			clks.clocks_in_khz[clks.num_levels*6/8], 1000);
	dc->bw_vbios->low_sclk  = bw_frc_to_fixed(
			clks.clocks_in_khz[0], 1000);
	dc->sclk_lvls = clks;

	/*do display clock*/
	dm_pp_get_clock_levels_by_type(
			dc->ctx,
			DM_PP_CLOCK_TYPE_DISPLAY_CLK,
			&clks);
	dc->bw_vbios->high_voltage_max_dispclk = bw_frc_to_fixed(
			clks.clocks_in_khz[clks.num_levels-1], 1000);
	dc->bw_vbios->mid_voltage_max_dispclk  = bw_frc_to_fixed(
			clks.clocks_in_khz[clks.num_levels>>1], 1000);
	dc->bw_vbios->low_voltage_max_dispclk  = bw_frc_to_fixed(
			clks.clocks_in_khz[0], 1000);

	/*do memory clock*/
	dm_pp_get_clock_levels_by_type(
			dc->ctx,
			DM_PP_CLOCK_TYPE_MEMORY_CLK,
			&clks);

	dc->bw_vbios->low_yclk = bw_frc_to_fixed(
		clks.clocks_in_khz[0] * MEMORY_TYPE_MULTIPLIER_CZ, 1000);
	dc->bw_vbios->mid_yclk = bw_frc_to_fixed(
		clks.clocks_in_khz[clks.num_levels>>1] * MEMORY_TYPE_MULTIPLIER_CZ,
		1000);
	dc->bw_vbios->high_yclk = bw_frc_to_fixed(
		clks.clocks_in_khz[clks.num_levels-1] * MEMORY_TYPE_MULTIPLIER_CZ,
		1000);
}