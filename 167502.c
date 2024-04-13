poppler_document_get_keywords (PopplerDocument *document)
{
  g_return_val_if_fail (POPPLER_IS_DOCUMENT (document), NULL);

  GooString *goo_keywords = document->doc->getDocInfoKeywords();
  gchar *utf8 = _poppler_goo_string_to_utf8(goo_keywords);
  delete goo_keywords;

  return utf8;
}