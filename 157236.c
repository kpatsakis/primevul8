static bool construct(
	uint8_t num_virtual_links,
	struct dc *dc,
	struct dcn10_resource_pool *pool)
{
	int i;
	int j;
	struct dc_context *ctx = dc->ctx;
	uint32_t pipe_fuses = read_pipe_fuses(ctx);

	ctx->dc_bios->regs = &bios_regs;

	if (ctx->dce_version == DCN_VERSION_1_01)
		pool->base.res_cap = &rv2_res_cap;
	else
		pool->base.res_cap = &res_cap;
	pool->base.funcs = &dcn10_res_pool_funcs;

	/*
	 * TODO fill in from actual raven resource when we create
	 * more than virtual encoder
	 */

	/*************************************************
	 *  Resource + asic cap harcoding                *
	 *************************************************/
	pool->base.underlay_pipe_index = NO_UNDERLAY_PIPE;

	/* max pipe num for ASIC before check pipe fuses */
	pool->base.pipe_count = pool->base.res_cap->num_timing_generator;

	if (dc->ctx->dce_version == DCN_VERSION_1_01)
		pool->base.pipe_count = 3;
	dc->caps.max_video_width = 3840;
	dc->caps.max_downscale_ratio = 200;
	dc->caps.i2c_speed_in_khz = 100;
	dc->caps.max_cursor_size = 256;
	dc->caps.max_slave_planes = 1;
	dc->caps.is_apu = true;
	dc->caps.post_blend_color_processing = false;
	/* Raven DP PHY HBR2 eye diagram pattern is not stable. Use TP4 */
	dc->caps.force_dp_tps4_for_cp2520 = true;

	if (dc->ctx->dce_environment == DCE_ENV_PRODUCTION_DRV)
		dc->debug = debug_defaults_drv;
	else
		dc->debug = debug_defaults_diags;

	/*************************************************
	 *  Create resources                             *
	 *************************************************/

	pool->base.clock_sources[DCN10_CLK_SRC_PLL0] =
			dcn10_clock_source_create(ctx, ctx->dc_bios,
				CLOCK_SOURCE_COMBO_PHY_PLL0,
				&clk_src_regs[0], false);
	pool->base.clock_sources[DCN10_CLK_SRC_PLL1] =
			dcn10_clock_source_create(ctx, ctx->dc_bios,
				CLOCK_SOURCE_COMBO_PHY_PLL1,
				&clk_src_regs[1], false);
	pool->base.clock_sources[DCN10_CLK_SRC_PLL2] =
			dcn10_clock_source_create(ctx, ctx->dc_bios,
				CLOCK_SOURCE_COMBO_PHY_PLL2,
				&clk_src_regs[2], false);

	if (dc->ctx->dce_version == DCN_VERSION_1_0) {
		pool->base.clock_sources[DCN10_CLK_SRC_PLL3] =
				dcn10_clock_source_create(ctx, ctx->dc_bios,
					CLOCK_SOURCE_COMBO_PHY_PLL3,
					&clk_src_regs[3], false);
	}

	pool->base.clk_src_count = DCN10_CLK_SRC_TOTAL;

	if (dc->ctx->dce_version == DCN_VERSION_1_01)
		pool->base.clk_src_count = DCN101_CLK_SRC_TOTAL;

	pool->base.dp_clock_source =
			dcn10_clock_source_create(ctx, ctx->dc_bios,
				CLOCK_SOURCE_ID_DP_DTO,
				/* todo: not reuse phy_pll registers */
				&clk_src_regs[0], true);

	for (i = 0; i < pool->base.clk_src_count; i++) {
		if (pool->base.clock_sources[i] == NULL) {
			dm_error("DC: failed to create clock sources!\n");
			BREAK_TO_DEBUGGER();
			goto fail;
		}
	}

	pool->base.dmcu = dcn10_dmcu_create(ctx,
			&dmcu_regs,
			&dmcu_shift,
			&dmcu_mask);
	if (pool->base.dmcu == NULL) {
		dm_error("DC: failed to create dmcu!\n");
		BREAK_TO_DEBUGGER();
		goto fail;
	}

	pool->base.abm = dce_abm_create(ctx,
			&abm_regs,
			&abm_shift,
			&abm_mask);
	if (pool->base.abm == NULL) {
		dm_error("DC: failed to create abm!\n");
		BREAK_TO_DEBUGGER();
		goto fail;
	}

	dml_init_instance(&dc->dml, &dcn1_0_soc, &dcn1_0_ip, DML_PROJECT_RAVEN1);
	memcpy(dc->dcn_ip, &dcn10_ip_defaults, sizeof(dcn10_ip_defaults));
	memcpy(dc->dcn_soc, &dcn10_soc_defaults, sizeof(dcn10_soc_defaults));

	if (dc->ctx->dce_version == DCN_VERSION_1_01) {
		struct dcn_soc_bounding_box *dcn_soc = dc->dcn_soc;
		struct dcn_ip_params *dcn_ip = dc->dcn_ip;
		struct display_mode_lib *dml = &dc->dml;

		dml->ip.max_num_dpp = 3;
		/* TODO how to handle 23.84? */
		dcn_soc->dram_clock_change_latency = 23;
		dcn_ip->max_num_dpp = 3;
	}
	if (ASICREV_IS_RV1_F0(dc->ctx->asic_id.hw_internal_rev)) {
		dc->dcn_soc->urgent_latency = 3;
		dc->debug.disable_dmcu = true;
		dc->dcn_soc->fabric_and_dram_bandwidth_vmax0p9 = 41.60f;
	}


	dc->dcn_soc->number_of_channels = dc->ctx->asic_id.vram_width / ddr4_dram_width;
	ASSERT(dc->dcn_soc->number_of_channels < 3);
	if (dc->dcn_soc->number_of_channels == 0)/*old sbios bug*/
		dc->dcn_soc->number_of_channels = 2;

	if (dc->dcn_soc->number_of_channels == 1) {
		dc->dcn_soc->fabric_and_dram_bandwidth_vmax0p9 = 19.2f;
		dc->dcn_soc->fabric_and_dram_bandwidth_vnom0p8 = 17.066f;
		dc->dcn_soc->fabric_and_dram_bandwidth_vmid0p72 = 14.933f;
		dc->dcn_soc->fabric_and_dram_bandwidth_vmin0p65 = 12.8f;
		if (ASICREV_IS_RV1_F0(dc->ctx->asic_id.hw_internal_rev)) {
			dc->dcn_soc->fabric_and_dram_bandwidth_vmax0p9 = 20.80f;
		}
	}

	pool->base.pp_smu = dcn10_pp_smu_create(ctx);

	/*
	 * Right now SMU/PPLIB and DAL all have the AZ D3 force PME notification *
	 * implemented. So AZ D3 should work.For issue 197007.                   *
	 */
	if (pool->base.pp_smu != NULL
			&& pool->base.pp_smu->rv_funcs.set_pme_wa_enable != NULL)
		dc->debug.az_endpoint_mute_only = false;

	if (!dc->debug.disable_pplib_clock_request)
		dcn_bw_update_from_pplib(dc);
	dcn_bw_sync_calcs_and_dml(dc);
	if (!dc->debug.disable_pplib_wm_range) {
		dc->res_pool = &pool->base;
		dcn_bw_notify_pplib_of_wm_ranges(dc);
	}

	{
		struct irq_service_init_data init_data;
		init_data.ctx = dc->ctx;
		pool->base.irqs = dal_irq_service_dcn10_create(&init_data);
		if (!pool->base.irqs)
			goto fail;
	}

	/* index to valid pipe resource  */
	j = 0;
	/* mem input -> ipp -> dpp -> opp -> TG */
	for (i = 0; i < pool->base.pipe_count; i++) {
		/* if pipe is disabled, skip instance of HW pipe,
		 * i.e, skip ASIC register instance
		 */
		if ((pipe_fuses & (1 << i)) != 0)
			continue;

		pool->base.hubps[j] = dcn10_hubp_create(ctx, i);
		if (pool->base.hubps[j] == NULL) {
			BREAK_TO_DEBUGGER();
			dm_error(
				"DC: failed to create memory input!\n");
			goto fail;
		}

		pool->base.ipps[j] = dcn10_ipp_create(ctx, i);
		if (pool->base.ipps[j] == NULL) {
			BREAK_TO_DEBUGGER();
			dm_error(
				"DC: failed to create input pixel processor!\n");
			goto fail;
		}

		pool->base.dpps[j] = dcn10_dpp_create(ctx, i);
		if (pool->base.dpps[j] == NULL) {
			BREAK_TO_DEBUGGER();
			dm_error(
				"DC: failed to create dpp!\n");
			goto fail;
		}

		pool->base.opps[j] = dcn10_opp_create(ctx, i);
		if (pool->base.opps[j] == NULL) {
			BREAK_TO_DEBUGGER();
			dm_error(
				"DC: failed to create output pixel processor!\n");
			goto fail;
		}

		pool->base.timing_generators[j] = dcn10_timing_generator_create(
				ctx, i);
		if (pool->base.timing_generators[j] == NULL) {
			BREAK_TO_DEBUGGER();
			dm_error("DC: failed to create tg!\n");
			goto fail;
		}
		/* check next valid pipe */
		j++;
	}

	for (i = 0; i < pool->base.res_cap->num_ddc; i++) {
		pool->base.engines[i] = dcn10_aux_engine_create(ctx, i);
		if (pool->base.engines[i] == NULL) {
			BREAK_TO_DEBUGGER();
			dm_error(
				"DC:failed to create aux engine!!\n");
			goto fail;
		}
		pool->base.hw_i2cs[i] = dcn10_i2c_hw_create(ctx, i);
		if (pool->base.hw_i2cs[i] == NULL) {
			BREAK_TO_DEBUGGER();
			dm_error(
				"DC:failed to create hw i2c!!\n");
			goto fail;
		}
		pool->base.sw_i2cs[i] = NULL;
	}

	/* valid pipe num */
	pool->base.pipe_count = j;
	pool->base.timing_generator_count = j;

	/* within dml lib, it is hard code to 4. If ASIC pipe is fused,
	 * the value may be changed
	 */
	dc->dml.ip.max_num_dpp = pool->base.pipe_count;
	dc->dcn_ip->max_num_dpp = pool->base.pipe_count;

	pool->base.mpc = dcn10_mpc_create(ctx);
	if (pool->base.mpc == NULL) {
		BREAK_TO_DEBUGGER();
		dm_error("DC: failed to create mpc!\n");
		goto fail;
	}

	pool->base.hubbub = dcn10_hubbub_create(ctx);
	if (pool->base.hubbub == NULL) {
		BREAK_TO_DEBUGGER();
		dm_error("DC: failed to create hubbub!\n");
		goto fail;
	}

	if (!resource_construct(num_virtual_links, dc, &pool->base,
			(!IS_FPGA_MAXIMUS_DC(dc->ctx->dce_environment) ?
			&res_create_funcs : &res_create_maximus_funcs)))
			goto fail;

	dcn10_hw_sequencer_construct(dc);
	dc->caps.max_planes =  pool->base.pipe_count;

	for (i = 0; i < dc->caps.max_planes; ++i)
		dc->caps.planes[i] = plane_cap;

	dc->cap_funcs = cap_funcs;

	return true;

fail:

	destruct(pool);

	return false;
}