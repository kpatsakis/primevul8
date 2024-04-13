static void bw_calcs_data_update_from_pplib(struct dc *dc)
{
	struct dm_pp_clock_levels_with_latency eng_clks = {0};
	struct dm_pp_clock_levels_with_latency mem_clks = {0};
	struct dm_pp_wm_sets_with_clock_ranges clk_ranges = {0};
	int i;
	unsigned int clk;
	unsigned int latency;
	/*original logic in dal3*/
	int memory_type_multiplier = MEMORY_TYPE_MULTIPLIER_CZ;

	/*do system clock*/
	if (!dm_pp_get_clock_levels_by_type_with_latency(
				dc->ctx,
				DM_PP_CLOCK_TYPE_ENGINE_CLK,
				&eng_clks) || eng_clks.num_levels == 0) {

		eng_clks.num_levels = 8;
		clk = 300000;

		for (i = 0; i < eng_clks.num_levels; i++) {
			eng_clks.data[i].clocks_in_khz = clk;
			clk += 100000;
		}
	}

	/* convert all the clock fro kHz to fix point mHz  TODO: wloop data */
	dc->bw_vbios->high_sclk = bw_frc_to_fixed(
		eng_clks.data[eng_clks.num_levels-1].clocks_in_khz, 1000);
	dc->bw_vbios->mid1_sclk  = bw_frc_to_fixed(
		eng_clks.data[eng_clks.num_levels/8].clocks_in_khz, 1000);
	dc->bw_vbios->mid2_sclk  = bw_frc_to_fixed(
		eng_clks.data[eng_clks.num_levels*2/8].clocks_in_khz, 1000);
	dc->bw_vbios->mid3_sclk  = bw_frc_to_fixed(
		eng_clks.data[eng_clks.num_levels*3/8].clocks_in_khz, 1000);
	dc->bw_vbios->mid4_sclk  = bw_frc_to_fixed(
		eng_clks.data[eng_clks.num_levels*4/8].clocks_in_khz, 1000);
	dc->bw_vbios->mid5_sclk  = bw_frc_to_fixed(
		eng_clks.data[eng_clks.num_levels*5/8].clocks_in_khz, 1000);
	dc->bw_vbios->mid6_sclk  = bw_frc_to_fixed(
		eng_clks.data[eng_clks.num_levels*6/8].clocks_in_khz, 1000);
	dc->bw_vbios->low_sclk  = bw_frc_to_fixed(
			eng_clks.data[0].clocks_in_khz, 1000);

	/*do memory clock*/
	if (!dm_pp_get_clock_levels_by_type_with_latency(
			dc->ctx,
			DM_PP_CLOCK_TYPE_MEMORY_CLK,
			&mem_clks) || mem_clks.num_levels == 0) {

		mem_clks.num_levels = 3;
		clk = 250000;
		latency = 45;

		for (i = 0; i < eng_clks.num_levels; i++) {
			mem_clks.data[i].clocks_in_khz = clk;
			mem_clks.data[i].latency_in_us = latency;
			clk += 500000;
			latency -= 5;
		}

	}

	/* we don't need to call PPLIB for validation clock since they
	 * also give us the highest sclk and highest mclk (UMA clock).
	 * ALSO always convert UMA clock (from PPLIB)  to YCLK (HW formula):
	 * YCLK = UMACLK*m_memoryTypeMultiplier
	 */
	if (dc->bw_vbios->memory_type == bw_def_hbm)
		memory_type_multiplier = MEMORY_TYPE_HBM;

	dc->bw_vbios->low_yclk = bw_frc_to_fixed(
		mem_clks.data[0].clocks_in_khz * memory_type_multiplier, 1000);
	dc->bw_vbios->mid_yclk = bw_frc_to_fixed(
		mem_clks.data[mem_clks.num_levels>>1].clocks_in_khz * memory_type_multiplier,
		1000);
	dc->bw_vbios->high_yclk = bw_frc_to_fixed(
		mem_clks.data[mem_clks.num_levels-1].clocks_in_khz * memory_type_multiplier,
		1000);

	/* Now notify PPLib/SMU about which Watermarks sets they should select
	 * depending on DPM state they are in. And update BW MGR GFX Engine and
	 * Memory clock member variables for Watermarks calculations for each
	 * Watermark Set
	 */
	clk_ranges.num_wm_sets = 4;
	clk_ranges.wm_clk_ranges[0].wm_set_id = WM_SET_A;
	clk_ranges.wm_clk_ranges[0].wm_min_eng_clk_in_khz =
			eng_clks.data[0].clocks_in_khz;
	clk_ranges.wm_clk_ranges[0].wm_max_eng_clk_in_khz =
			eng_clks.data[eng_clks.num_levels*3/8].clocks_in_khz - 1;
	clk_ranges.wm_clk_ranges[0].wm_min_mem_clk_in_khz =
			mem_clks.data[0].clocks_in_khz;
	clk_ranges.wm_clk_ranges[0].wm_max_mem_clk_in_khz =
			mem_clks.data[mem_clks.num_levels>>1].clocks_in_khz - 1;

	clk_ranges.wm_clk_ranges[1].wm_set_id = WM_SET_B;
	clk_ranges.wm_clk_ranges[1].wm_min_eng_clk_in_khz =
			eng_clks.data[eng_clks.num_levels*3/8].clocks_in_khz;
	/* 5 GHz instead of data[7].clockInKHz to cover Overdrive */
	clk_ranges.wm_clk_ranges[1].wm_max_eng_clk_in_khz = 5000000;
	clk_ranges.wm_clk_ranges[1].wm_min_mem_clk_in_khz =
			mem_clks.data[0].clocks_in_khz;
	clk_ranges.wm_clk_ranges[1].wm_max_mem_clk_in_khz =
			mem_clks.data[mem_clks.num_levels>>1].clocks_in_khz - 1;

	clk_ranges.wm_clk_ranges[2].wm_set_id = WM_SET_C;
	clk_ranges.wm_clk_ranges[2].wm_min_eng_clk_in_khz =
			eng_clks.data[0].clocks_in_khz;
	clk_ranges.wm_clk_ranges[2].wm_max_eng_clk_in_khz =
			eng_clks.data[eng_clks.num_levels*3/8].clocks_in_khz - 1;
	clk_ranges.wm_clk_ranges[2].wm_min_mem_clk_in_khz =
			mem_clks.data[mem_clks.num_levels>>1].clocks_in_khz;
	/* 5 GHz instead of data[2].clockInKHz to cover Overdrive */
	clk_ranges.wm_clk_ranges[2].wm_max_mem_clk_in_khz = 5000000;

	clk_ranges.wm_clk_ranges[3].wm_set_id = WM_SET_D;
	clk_ranges.wm_clk_ranges[3].wm_min_eng_clk_in_khz =
			eng_clks.data[eng_clks.num_levels*3/8].clocks_in_khz;
	/* 5 GHz instead of data[7].clockInKHz to cover Overdrive */
	clk_ranges.wm_clk_ranges[3].wm_max_eng_clk_in_khz = 5000000;
	clk_ranges.wm_clk_ranges[3].wm_min_mem_clk_in_khz =
			mem_clks.data[mem_clks.num_levels>>1].clocks_in_khz;
	/* 5 GHz instead of data[2].clockInKHz to cover Overdrive */
	clk_ranges.wm_clk_ranges[3].wm_max_mem_clk_in_khz = 5000000;

	/* Notify PP Lib/SMU which Watermarks to use for which clock ranges */
	dm_pp_notify_wm_clock_changes(dc->ctx, &clk_ranges);
}