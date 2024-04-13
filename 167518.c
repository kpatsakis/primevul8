poppler_document_get_subject (PopplerDocument *document)
{
  g_return_val_if_fail (POPPLER_IS_DOCUMENT (document), NULL);

  GooString *goo_subject = document->doc->getDocInfoSubject();
  gchar *utf8 = _poppler_goo_string_to_utf8(goo_subject);
  delete goo_subject;

  return utf8;
}