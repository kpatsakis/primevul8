fz_default_output_intent(fz_context *ctx, const fz_default_colorspaces *default_cs)
{
	if (default_cs)
		return default_cs->oi;
	else
		return NULL;
}