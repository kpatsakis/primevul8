poppler_document_get_producer (PopplerDocument *document)
{
  g_return_val_if_fail (POPPLER_IS_DOCUMENT (document), NULL);

  GooString *goo_producer = document->doc->getDocInfoProducer();
  gchar *utf8 = _poppler_goo_string_to_utf8(goo_producer);
  delete goo_producer;

  return utf8;
}