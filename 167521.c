poppler_document_get_metadata (PopplerDocument *document)
{
  Catalog *catalog;
  gchar *retval = nullptr;

  g_return_val_if_fail (POPPLER_IS_DOCUMENT (document), NULL);

  catalog = document->doc->getCatalog ();
  if (catalog && catalog->isOk ()) {
    GooString *s = catalog->readMetadata ();

    if (s != nullptr) {
      retval = g_strdup (s->getCString());
      delete s;
    }
  }

  return retval;
}