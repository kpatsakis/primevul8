poppler_document_is_linearized (PopplerDocument *document)
{
  g_return_val_if_fail (POPPLER_IS_DOCUMENT (document), FALSE);

  return document->doc->isLinearized ();
}