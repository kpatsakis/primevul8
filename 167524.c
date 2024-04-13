poppler_fonts_iter_get_file_name (PopplerFontsIter *iter)
{
	GooString *file;
	FontInfo *info;

	info = (FontInfo *)iter->items->get (iter->index);

	file = info->getFile();
	if (file != nullptr) {
		return file->getCString();
	} else {
		return nullptr;
	}
}