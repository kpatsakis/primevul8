poppler_fonts_iter_get_name (PopplerFontsIter *iter)
{
	FontInfo *info;
	const char *name;

	name = poppler_fonts_iter_get_full_name (iter);
	info = (FontInfo *)iter->items->get (iter->index);

	if (info->getSubset() && name) {
		while (*name && *name != '+')
			name++;

		if (*name)
			name++;
	}

	return name;
}