static bool dce110_validate_bandwidth(
	struct dc *dc,
	struct dc_state *context,
	bool fast_validate)
{
	bool result = false;

	DC_LOG_BANDWIDTH_CALCS(
		"%s: start",
		__func__);

	if (bw_calcs(
			dc->ctx,
			dc->bw_dceip,
			dc->bw_vbios,
			context->res_ctx.pipe_ctx,
			dc->res_pool->pipe_count,
			&context->bw_ctx.bw.dce))
		result =  true;

	if (!result)
		DC_LOG_BANDWIDTH_VALIDATION("%s: %dx%d@%d Bandwidth validation failed!\n",
			__func__,
			context->streams[0]->timing.h_addressable,
			context->streams[0]->timing.v_addressable,
			context->streams[0]->timing.pix_clk_100hz / 10);

	if (memcmp(&dc->current_state->bw_ctx.bw.dce,
			&context->bw_ctx.bw.dce, sizeof(context->bw_ctx.bw.dce))) {

		DC_LOG_BANDWIDTH_CALCS(
			"%s: finish,\n"
			"nbpMark_b: %d nbpMark_a: %d urgentMark_b: %d urgentMark_a: %d\n"
			"stutMark_b: %d stutMark_a: %d\n"
			"nbpMark_b: %d nbpMark_a: %d urgentMark_b: %d urgentMark_a: %d\n"
			"stutMark_b: %d stutMark_a: %d\n"
			"nbpMark_b: %d nbpMark_a: %d urgentMark_b: %d urgentMark_a: %d\n"
			"stutMark_b: %d stutMark_a: %d stutter_mode_enable: %d\n"
			"cstate: %d pstate: %d nbpstate: %d sync: %d dispclk: %d\n"
			"sclk: %d sclk_sleep: %d yclk: %d blackout_recovery_time_us: %d\n"
			,
			__func__,
			context->bw_ctx.bw.dce.nbp_state_change_wm_ns[0].b_mark,
			context->bw_ctx.bw.dce.nbp_state_change_wm_ns[0].a_mark,
			context->bw_ctx.bw.dce.urgent_wm_ns[0].b_mark,
			context->bw_ctx.bw.dce.urgent_wm_ns[0].a_mark,
			context->bw_ctx.bw.dce.stutter_exit_wm_ns[0].b_mark,
			context->bw_ctx.bw.dce.stutter_exit_wm_ns[0].a_mark,
			context->bw_ctx.bw.dce.nbp_state_change_wm_ns[1].b_mark,
			context->bw_ctx.bw.dce.nbp_state_change_wm_ns[1].a_mark,
			context->bw_ctx.bw.dce.urgent_wm_ns[1].b_mark,
			context->bw_ctx.bw.dce.urgent_wm_ns[1].a_mark,
			context->bw_ctx.bw.dce.stutter_exit_wm_ns[1].b_mark,
			context->bw_ctx.bw.dce.stutter_exit_wm_ns[1].a_mark,
			context->bw_ctx.bw.dce.nbp_state_change_wm_ns[2].b_mark,
			context->bw_ctx.bw.dce.nbp_state_change_wm_ns[2].a_mark,
			context->bw_ctx.bw.dce.urgent_wm_ns[2].b_mark,
			context->bw_ctx.bw.dce.urgent_wm_ns[2].a_mark,
			context->bw_ctx.bw.dce.stutter_exit_wm_ns[2].b_mark,
			context->bw_ctx.bw.dce.stutter_exit_wm_ns[2].a_mark,
			context->bw_ctx.bw.dce.stutter_mode_enable,
			context->bw_ctx.bw.dce.cpuc_state_change_enable,
			context->bw_ctx.bw.dce.cpup_state_change_enable,
			context->bw_ctx.bw.dce.nbp_state_change_enable,
			context->bw_ctx.bw.dce.all_displays_in_sync,
			context->bw_ctx.bw.dce.dispclk_khz,
			context->bw_ctx.bw.dce.sclk_khz,
			context->bw_ctx.bw.dce.sclk_deep_sleep_khz,
			context->bw_ctx.bw.dce.yclk_khz,
			context->bw_ctx.bw.dce.blackout_recovery_time_us);
	}
	return result;
}