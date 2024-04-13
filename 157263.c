static bool construct(
	uint8_t num_virtual_links,
	struct dc *dc,
	struct dce110_resource_pool *pool)
{
	unsigned int i;
	int j;
	struct dc_context *ctx = dc->ctx;
	struct irq_service_init_data irq_init_data;
	static const struct resource_create_funcs *res_funcs;
	bool is_vg20 = ASICREV_IS_VEGA20_P(ctx->asic_id.hw_internal_rev);
	uint32_t pipe_fuses;

	ctx->dc_bios->regs = &bios_regs;

	pool->base.res_cap = &res_cap;
	pool->base.funcs = &dce120_res_pool_funcs;

	/* TODO: Fill more data from GreenlandAsicCapability.cpp */
	pool->base.pipe_count = res_cap.num_timing_generator;
	pool->base.timing_generator_count = pool->base.res_cap->num_timing_generator;
	pool->base.underlay_pipe_index = NO_UNDERLAY_PIPE;

	dc->caps.max_downscale_ratio = 200;
	dc->caps.i2c_speed_in_khz = 100;
	dc->caps.max_cursor_size = 128;
	dc->caps.dual_link_dvi = true;
	dc->caps.psp_setup_panel_mode = true;

	dc->debug = debug_defaults;

	/*************************************************
	 *  Create resources                             *
	 *************************************************/

	pool->base.clock_sources[DCE120_CLK_SRC_PLL0] =
			dce120_clock_source_create(ctx, ctx->dc_bios,
				CLOCK_SOURCE_COMBO_PHY_PLL0,
				&clk_src_regs[0], false);
	pool->base.clock_sources[DCE120_CLK_SRC_PLL1] =
			dce120_clock_source_create(ctx, ctx->dc_bios,
				CLOCK_SOURCE_COMBO_PHY_PLL1,
				&clk_src_regs[1], false);
	pool->base.clock_sources[DCE120_CLK_SRC_PLL2] =
			dce120_clock_source_create(ctx, ctx->dc_bios,
				CLOCK_SOURCE_COMBO_PHY_PLL2,
				&clk_src_regs[2], false);
	pool->base.clock_sources[DCE120_CLK_SRC_PLL3] =
			dce120_clock_source_create(ctx, ctx->dc_bios,
				CLOCK_SOURCE_COMBO_PHY_PLL3,
				&clk_src_regs[3], false);
	pool->base.clock_sources[DCE120_CLK_SRC_PLL4] =
			dce120_clock_source_create(ctx, ctx->dc_bios,
				CLOCK_SOURCE_COMBO_PHY_PLL4,
				&clk_src_regs[4], false);
	pool->base.clock_sources[DCE120_CLK_SRC_PLL5] =
			dce120_clock_source_create(ctx, ctx->dc_bios,
				CLOCK_SOURCE_COMBO_PHY_PLL5,
				&clk_src_regs[5], false);
	pool->base.clk_src_count = DCE120_CLK_SRC_TOTAL;

	pool->base.dp_clock_source =
			dce120_clock_source_create(ctx, ctx->dc_bios,
				CLOCK_SOURCE_ID_DP_DTO,
				&clk_src_regs[0], true);

	for (i = 0; i < pool->base.clk_src_count; i++) {
		if (pool->base.clock_sources[i] == NULL) {
			dm_error("DC: failed to create clock sources!\n");
			BREAK_TO_DEBUGGER();
			goto clk_src_create_fail;
		}
	}

	pool->base.dmcu = dce_dmcu_create(ctx,
			&dmcu_regs,
			&dmcu_shift,
			&dmcu_mask);
	if (pool->base.dmcu == NULL) {
		dm_error("DC: failed to create dmcu!\n");
		BREAK_TO_DEBUGGER();
		goto res_create_fail;
	}

	pool->base.abm = dce_abm_create(ctx,
			&abm_regs,
			&abm_shift,
			&abm_mask);
	if (pool->base.abm == NULL) {
		dm_error("DC: failed to create abm!\n");
		BREAK_TO_DEBUGGER();
		goto res_create_fail;
	}


	irq_init_data.ctx = dc->ctx;
	pool->base.irqs = dal_irq_service_dce120_create(&irq_init_data);
	if (!pool->base.irqs)
		goto irqs_create_fail;

	/* VG20: Pipe harvesting enabled, retrieve valid pipe fuses */
	if (is_vg20)
		pipe_fuses = read_pipe_fuses(ctx);

	/* index to valid pipe resource */
	j = 0;
	for (i = 0; i < pool->base.pipe_count; i++) {
		if (is_vg20) {
			if ((pipe_fuses & (1 << i)) != 0) {
				dm_error("DC: skip invalid pipe %d!\n", i);
				continue;
			}
		}

		pool->base.timing_generators[j] =
				dce120_timing_generator_create(
					ctx,
					i,
					&dce120_tg_offsets[i]);
		if (pool->base.timing_generators[j] == NULL) {
			BREAK_TO_DEBUGGER();
			dm_error("DC: failed to create tg!\n");
			goto controller_create_fail;
		}

		pool->base.mis[j] = dce120_mem_input_create(ctx, i);

		if (pool->base.mis[j] == NULL) {
			BREAK_TO_DEBUGGER();
			dm_error(
				"DC: failed to create memory input!\n");
			goto controller_create_fail;
		}

		pool->base.ipps[j] = dce120_ipp_create(ctx, i);
		if (pool->base.ipps[i] == NULL) {
			BREAK_TO_DEBUGGER();
			dm_error(
				"DC: failed to create input pixel processor!\n");
			goto controller_create_fail;
		}

		pool->base.transforms[j] = dce120_transform_create(ctx, i);
		if (pool->base.transforms[i] == NULL) {
			BREAK_TO_DEBUGGER();
			dm_error(
				"DC: failed to create transform!\n");
			goto res_create_fail;
		}

		pool->base.opps[j] = dce120_opp_create(
			ctx,
			i);
		if (pool->base.opps[j] == NULL) {
			BREAK_TO_DEBUGGER();
			dm_error(
				"DC: failed to create output pixel processor!\n");
		}

		/* check next valid pipe */
		j++;
	}

	for (i = 0; i < pool->base.res_cap->num_ddc; i++) {
		pool->base.engines[i] = dce120_aux_engine_create(ctx, i);
		if (pool->base.engines[i] == NULL) {
			BREAK_TO_DEBUGGER();
			dm_error(
				"DC:failed to create aux engine!!\n");
			goto res_create_fail;
		}
		pool->base.hw_i2cs[i] = dce120_i2c_hw_create(ctx, i);
		if (pool->base.hw_i2cs[i] == NULL) {
			BREAK_TO_DEBUGGER();
			dm_error(
				"DC:failed to create i2c engine!!\n");
			goto res_create_fail;
		}
		pool->base.sw_i2cs[i] = NULL;
	}

	/* valid pipe num */
	pool->base.pipe_count = j;
	pool->base.timing_generator_count = j;

	if (is_vg20)
		res_funcs = &dce121_res_create_funcs;
	else
		res_funcs = &res_create_funcs;

	if (!resource_construct(num_virtual_links, dc, &pool->base, res_funcs))
		goto res_create_fail;

	/* Create hardware sequencer */
	if (!dce120_hw_sequencer_create(dc))
		goto controller_create_fail;

	dc->caps.max_planes =  pool->base.pipe_count;

	for (i = 0; i < dc->caps.max_planes; ++i)
		dc->caps.planes[i] = plane_cap;

	bw_calcs_init(dc->bw_dceip, dc->bw_vbios, dc->ctx->asic_id);

	bw_calcs_data_update_from_pplib(dc);

	return true;

irqs_create_fail:
controller_create_fail:
clk_src_create_fail:
res_create_fail:

	destruct(pool);

	return false;
}