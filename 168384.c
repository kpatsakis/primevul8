fz_drop_color_converter(fz_context *ctx, fz_color_converter *cc)
{
	fz_icclink *link = (fz_icclink *)cc->link;
	if (link)
		fz_drop_icclink(ctx, link);
	cc->link = NULL;

}