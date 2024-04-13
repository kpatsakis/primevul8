fz_drop_icclink(fz_context *ctx, fz_icclink *link)
{
	fz_drop_storable(ctx, &link->storable);
}