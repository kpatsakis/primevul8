poppler_document_set_title (PopplerDocument *document, const gchar *title)
{
  g_return_if_fail (POPPLER_IS_DOCUMENT (document));

  GooString *goo_title;
  if (!title) {
    goo_title = nullptr;
  } else {
    goo_title = _poppler_goo_string_from_utf8(title);
    if (!goo_title)
      return;
  }
  document->doc->setDocInfoTitle(goo_title);
}