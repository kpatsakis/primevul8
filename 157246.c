static bool construct(
	uint8_t num_virtual_links,
	struct dc *dc,
	struct dce110_resource_pool *pool,
	struct hw_asic_id asic_id)
{
	unsigned int i;
	struct dc_context *ctx = dc->ctx;
	struct dc_bios *bp;

	ctx->dc_bios->regs = &bios_regs;

	pool->base.res_cap = dce110_resource_cap(&ctx->asic_id);
	pool->base.funcs = &dce110_res_pool_funcs;

	/*************************************************
	 *  Resource + asic cap harcoding                *
	 *************************************************/

	pool->base.pipe_count = pool->base.res_cap->num_timing_generator;
	pool->base.underlay_pipe_index = pool->base.pipe_count;
	pool->base.timing_generator_count = pool->base.res_cap->num_timing_generator;
	dc->caps.max_downscale_ratio = 150;
	dc->caps.i2c_speed_in_khz = 100;
	dc->caps.max_cursor_size = 128;
	dc->caps.is_apu = true;

	/*************************************************
	 *  Create resources                             *
	 *************************************************/

	bp = ctx->dc_bios;

	if (bp->fw_info_valid && bp->fw_info.external_clock_source_frequency_for_dp != 0) {
		pool->base.dp_clock_source =
				dce110_clock_source_create(ctx, bp, CLOCK_SOURCE_ID_EXTERNAL, NULL, true);

		pool->base.clock_sources[0] =
				dce110_clock_source_create(ctx, bp, CLOCK_SOURCE_ID_PLL0,
						&clk_src_regs[0], false);
		pool->base.clock_sources[1] =
				dce110_clock_source_create(ctx, bp, CLOCK_SOURCE_ID_PLL1,
						&clk_src_regs[1], false);

		pool->base.clk_src_count = 2;

		/* TODO: find out if CZ support 3 PLLs */
	}

	if (pool->base.dp_clock_source == NULL) {
		dm_error("DC: failed to create dp clock source!\n");
		BREAK_TO_DEBUGGER();
		goto res_create_fail;
	}

	for (i = 0; i < pool->base.clk_src_count; i++) {
		if (pool->base.clock_sources[i] == NULL) {
			dm_error("DC: failed to create clock sources!\n");
			BREAK_TO_DEBUGGER();
			goto res_create_fail;
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

	{
		struct irq_service_init_data init_data;
		init_data.ctx = dc->ctx;
		pool->base.irqs = dal_irq_service_dce110_create(&init_data);
		if (!pool->base.irqs)
			goto res_create_fail;
	}

	for (i = 0; i < pool->base.pipe_count; i++) {
		pool->base.timing_generators[i] = dce110_timing_generator_create(
				ctx, i, &dce110_tg_offsets[i]);
		if (pool->base.timing_generators[i] == NULL) {
			BREAK_TO_DEBUGGER();
			dm_error("DC: failed to create tg!\n");
			goto res_create_fail;
		}

		pool->base.mis[i] = dce110_mem_input_create(ctx, i);
		if (pool->base.mis[i] == NULL) {
			BREAK_TO_DEBUGGER();
			dm_error(
				"DC: failed to create memory input!\n");
			goto res_create_fail;
		}

		pool->base.ipps[i] = dce110_ipp_create(ctx, i);
		if (pool->base.ipps[i] == NULL) {
			BREAK_TO_DEBUGGER();
			dm_error(
				"DC: failed to create input pixel processor!\n");
			goto res_create_fail;
		}

		pool->base.transforms[i] = dce110_transform_create(ctx, i);
		if (pool->base.transforms[i] == NULL) {
			BREAK_TO_DEBUGGER();
			dm_error(
				"DC: failed to create transform!\n");
			goto res_create_fail;
		}

		pool->base.opps[i] = dce110_opp_create(ctx, i);
		if (pool->base.opps[i] == NULL) {
			BREAK_TO_DEBUGGER();
			dm_error(
				"DC: failed to create output pixel processor!\n");
			goto res_create_fail;
		}
	}

	for (i = 0; i < pool->base.res_cap->num_ddc; i++) {
		pool->base.engines[i] = dce110_aux_engine_create(ctx, i);
		if (pool->base.engines[i] == NULL) {
			BREAK_TO_DEBUGGER();
			dm_error(
				"DC:failed to create aux engine!!\n");
			goto res_create_fail;
		}
		pool->base.hw_i2cs[i] = dce110_i2c_hw_create(ctx, i);
		if (pool->base.hw_i2cs[i] == NULL) {
			BREAK_TO_DEBUGGER();
			dm_error(
				"DC:failed to create i2c engine!!\n");
			goto res_create_fail;
		}
		pool->base.sw_i2cs[i] = NULL;
	}

	if (dc->config.fbc_support)
		dc->fbc_compressor = dce110_compressor_create(ctx);

	if (!underlay_create(ctx, &pool->base))
		goto res_create_fail;

	if (!resource_construct(num_virtual_links, dc, &pool->base,
			&res_create_funcs))
		goto res_create_fail;

	/* Create hardware sequencer */
	dce110_hw_sequencer_construct(dc);

	dc->caps.max_planes =  pool->base.pipe_count;

	for (i = 0; i < pool->base.underlay_pipe_index; ++i)
		dc->caps.planes[i] = plane_cap;

	dc->caps.planes[pool->base.underlay_pipe_index] = underlay_plane_cap;

	bw_calcs_init(dc->bw_dceip, dc->bw_vbios, dc->ctx->asic_id);

	bw_calcs_data_update_from_pplib(dc);

	return true;

res_create_fail:
	destruct(pool);
	return false;
}