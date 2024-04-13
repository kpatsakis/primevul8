poppler_document_set_keywords (PopplerDocument *document, const gchar *keywords)
{
  g_return_if_fail (POPPLER_IS_DOCUMENT (document));

  GooString *goo_keywords;
  if (!keywords) {
    goo_keywords = nullptr;
  } else {
    goo_keywords = _poppler_goo_string_from_utf8(keywords);
    if (!goo_keywords)
      return;
  }
  document->doc->setDocInfoKeywords(goo_keywords);
}