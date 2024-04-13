poppler_document_set_author (PopplerDocument *document, const gchar *author)
{
  g_return_if_fail (POPPLER_IS_DOCUMENT (document));

  GooString *goo_author;
  if (!author) {
    goo_author = nullptr;
  } else {
    goo_author = _poppler_goo_string_from_utf8(author);
    if (!goo_author)
      return;
  }
  document->doc->setDocInfoAuthor(goo_author);
}