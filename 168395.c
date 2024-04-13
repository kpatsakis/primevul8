fz_lookup_rendering_intent(const char *name)
{
	int i;
	for (i = 0; i < nelem(fz_intent_names); i++)
		if (!strcmp(name, fz_intent_names[i]))
			return i;
	return FZ_RI_RELATIVE_COLORIMETRIC;
}