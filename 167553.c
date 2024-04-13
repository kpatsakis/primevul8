poppler_document_set_subject (PopplerDocument *document, const gchar *subject)
{
  g_return_if_fail (POPPLER_IS_DOCUMENT (document));

  GooString *goo_subject;
  if (!subject) {
    goo_subject = nullptr;
  } else {
    goo_subject = _poppler_goo_string_from_utf8(subject);
    if (!goo_subject)
      return;
  }
  document->doc->setDocInfoSubject(goo_subject);
}