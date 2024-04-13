fz_rendering_intent_name(int ri)
{
	if (ri >= 0 && ri < nelem(fz_intent_names))
		return (char*)fz_intent_names[ri];
	return "RelativeColorimetric";
}