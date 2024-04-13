poppler_fonts_iter_get_encoding (PopplerFontsIter *iter)
{
	GooString *encoding;
	FontInfo *info;

	info = (FontInfo *)iter->items->get (iter->index);

	encoding = info->getEncoding();
	if (encoding != nullptr) {
		return encoding->getCString();
	} else {
		return nullptr;
	}
}