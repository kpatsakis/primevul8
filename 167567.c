poppler_document_has_attachments (PopplerDocument *document)
{
  g_return_val_if_fail (POPPLER_IS_DOCUMENT (document), FALSE);

  return (poppler_document_get_n_attachments (document) != 0);
}