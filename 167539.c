poppler_document_set_producer (PopplerDocument *document, const gchar *producer)
{
  g_return_if_fail (POPPLER_IS_DOCUMENT (document));

  GooString *goo_producer;
  if (!producer) {
    goo_producer = nullptr;
  } else {
    goo_producer = _poppler_goo_string_from_utf8(producer);
    if (!goo_producer)
      return;
  }
  document->doc->setDocInfoProducer(goo_producer);
}