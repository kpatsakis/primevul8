poppler_document_get_author (PopplerDocument *document)
{
  g_return_val_if_fail (POPPLER_IS_DOCUMENT (document), NULL);

  GooString *goo_author = document->doc->getDocInfoAuthor();
  gchar *utf8 = _poppler_goo_string_to_utf8(goo_author);
  delete goo_author;

  return utf8;
}