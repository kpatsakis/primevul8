poppler_font_info_scan (PopplerFontInfo   *font_info,
			int                n_pages,
			PopplerFontsIter **iter)
{
	GooList *items;

	g_return_val_if_fail (iter != nullptr, FALSE);

	items = font_info->scanner->scan(n_pages);

	if (items == nullptr) {
		*iter = nullptr;
	} else if (items->getLength() == 0) {
		*iter = nullptr;
		delete items;
	} else {
		*iter = poppler_fonts_iter_new(items);
	}
	
	return (items != nullptr);
}