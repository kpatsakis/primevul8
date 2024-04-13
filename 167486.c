poppler_document_get_page_layout (PopplerDocument *document)
{
  Catalog *catalog;

  g_return_val_if_fail (POPPLER_IS_DOCUMENT (document), POPPLER_PAGE_LAYOUT_UNSET);

  catalog = document->doc->getCatalog ();
  if (catalog && catalog->isOk ())
    return convert_page_layout (catalog->getPageLayout ());

  return POPPLER_PAGE_LAYOUT_UNSET;
}