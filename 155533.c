pdf_init_gstate(fz_context *ctx, pdf_gstate *gs, const fz_matrix *ctm)
{
	gs->ctm = *ctm;
	gs->clip_depth = 0;

	gs->stroke_state = fz_new_stroke_state(ctx);

	gs->stroke.kind = PDF_MAT_COLOR;
	gs->stroke.colorspace = fz_keep_colorspace(ctx, fz_device_gray(ctx));
	gs->stroke.v[0] = 0;
	gs->stroke.pattern = NULL;
	gs->stroke.shade = NULL;
	gs->stroke.alpha = 1;
	gs->stroke.gstate_num = -1;

	gs->fill.kind = PDF_MAT_COLOR;
	gs->fill.colorspace = fz_keep_colorspace(ctx, fz_device_gray(ctx));
	gs->fill.v[0] = 0;
	gs->fill.pattern = NULL;
	gs->fill.shade = NULL;
	gs->fill.alpha = 1;
	gs->fill.gstate_num = -1;

	gs->text.char_space = 0;
	gs->text.word_space = 0;
	gs->text.scale = 1;
	gs->text.leading = 0;
	gs->text.font = NULL;
	gs->text.size = -1;
	gs->text.render = 0;
	gs->text.rise = 0;

	gs->blendmode = 0;
	gs->softmask = NULL;
	gs->softmask_resources = NULL;
	gs->softmask_ctm = fz_identity;
	gs->luminosity = 0;

	gs->fill.color_params = *fz_default_color_params(ctx);
	gs->stroke.color_params = *fz_default_color_params(ctx);
}