poppler_document_get_title (PopplerDocument *document)
{
  g_return_val_if_fail (POPPLER_IS_DOCUMENT (document), NULL);

  GooString *goo_title = document->doc->getDocInfoTitle();
  gchar *utf8 = _poppler_goo_string_to_utf8(goo_title);
  delete goo_title;

  return utf8;
}