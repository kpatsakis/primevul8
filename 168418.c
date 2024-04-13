fz_drop_link_imp(fz_context *ctx, fz_storable *storable)
{
	fz_icclink *link = (fz_icclink *)storable;
	fz_cmm_fin_link(ctx, link);
	fz_free(ctx, link);
}