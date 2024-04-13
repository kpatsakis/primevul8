poppler_document_set_creator (PopplerDocument *document, const gchar *creator)
{
  g_return_if_fail (POPPLER_IS_DOCUMENT (document));

  GooString *goo_creator;
  if (!creator) {
    goo_creator = nullptr;
  } else {
    goo_creator = _poppler_goo_string_from_utf8(creator);
    if (!goo_creator)
      return;
  }
  document->doc->setDocInfoCreator(goo_creator);
}