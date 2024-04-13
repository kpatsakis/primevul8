poppler_fonts_iter_get_substitute_name (PopplerFontsIter *iter)
{
	GooString *name;
	FontInfo *info;

	info = (FontInfo *)iter->items->get (iter->index);

	name = info->getSubstituteName();
	if (name != nullptr) {
		return name->getCString();
	} else {
		return nullptr;
	}
}