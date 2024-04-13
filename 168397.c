void fz_colorspace_name_colorant(fz_context *ctx, fz_colorspace *cs, int i, const char *name)
{
	if (!cs)
		return;

	if (i < 0 || i >= cs->n)
		fz_throw(ctx, FZ_ERROR_GENERIC, "Attempt to name out of range colorant");

	fz_free(ctx, cs->colorant[i]);
	cs->colorant[i] = NULL;
	if (name)
	{
		cs->colorant[i] = fz_strdup(ctx, name);

		if (cs->type == FZ_COLORSPACE_SEPARATION)
		{
			if (i == 0)
			{
				if (strcmp(name, "Cyan") == 0 ||
					strcmp(name, "Magenta") == 0 ||
					strcmp(name, "Yellow") == 0 ||
					strcmp(name, "Black") == 0)
				{
					cs->flags |= FZ_CS_HAS_CMYK;
				}
			}
			else
			{
				if ((cs->flags & FZ_CS_HAS_CMYK_AND_SPOTS) != FZ_CS_HAS_CMYK_AND_SPOTS)
				{
					if (strcmp(name, "Cyan") == 0 ||
						strcmp(name, "Magenta") == 0 ||
						strcmp(name, "Yellow") == 0 ||
						strcmp(name, "Black") == 0)
						cs->flags |= FZ_CS_HAS_CMYK;
					else
						cs->flags |= FZ_CS_HAS_SPOTS;
				}
			}
		}
	}
}