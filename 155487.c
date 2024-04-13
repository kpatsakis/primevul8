pdf_copy_gstate(fz_context *ctx, pdf_gstate *gs, pdf_gstate *old)
{
	pdf_drop_gstate(ctx, gs);
	*gs = *old;
	pdf_keep_gstate(ctx, gs);
}