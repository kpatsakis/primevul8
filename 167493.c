poppler_document_get_n_attachments (PopplerDocument *document)
{
  Catalog *catalog;

  g_return_val_if_fail (POPPLER_IS_DOCUMENT (document), 0);

  catalog = document->doc->getCatalog ();

  return catalog && catalog->isOk () ? catalog->numEmbeddedFiles () : 0;
}