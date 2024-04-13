poppler_document_get_page (PopplerDocument  *document,
			   int               index)
{
  Page *page;

  g_return_val_if_fail (0 <= index &&
			index < poppler_document_get_n_pages (document),
			NULL);

  page = document->doc->getPage (index + 1);
  if (!page) return nullptr;

  return _poppler_page_new (document, page, index);
}