poppler_document_get_page_mode (PopplerDocument *document)
{
  Catalog *catalog;

  g_return_val_if_fail (POPPLER_IS_DOCUMENT (document), POPPLER_PAGE_MODE_UNSET);

  catalog = document->doc->getCatalog ();
  if (catalog && catalog->isOk ())
    return convert_page_mode (catalog->getPageMode ());

  return POPPLER_PAGE_MODE_UNSET;
}