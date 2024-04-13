poppler_document_get_pdf_version_string (PopplerDocument *document)
{
  gchar *retval;

  g_return_val_if_fail (POPPLER_IS_DOCUMENT (document), NULL);

  retval = g_strndup ("PDF-", 15); /* allocates 16 chars, pads with \0s */
  g_ascii_formatd (retval + 4, 15 + 1 - 4, "%.2g",
		   document->doc->getPDFMajorVersion () + document->doc->getPDFMinorVersion() / 10.0);
  return retval;
}