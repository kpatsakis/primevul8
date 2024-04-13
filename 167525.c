poppler_fonts_iter_get_full_name (PopplerFontsIter *iter)
{
	GooString *name;
	FontInfo *info;

	info = (FontInfo *)iter->items->get (iter->index);

	name = info->getName();
	if (name != nullptr) {
		return info->getName()->getCString();
	} else {
		return nullptr;
	}
}