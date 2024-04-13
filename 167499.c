poppler_document_get_creator (PopplerDocument *document)
{
  g_return_val_if_fail (POPPLER_IS_DOCUMENT (document), NULL);

  GooString *goo_creator = document->doc->getDocInfoCreator();
  gchar *utf8 = _poppler_goo_string_to_utf8(goo_creator);
  delete goo_creator;

  return utf8;
}