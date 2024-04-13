poppler_document_get_pdf_version (PopplerDocument *document,
				  guint           *major_version,
				  guint           *minor_version)
{
  g_return_if_fail (POPPLER_IS_DOCUMENT (document));

  if (major_version)
    *major_version = document->doc->getPDFMajorVersion ();
  if (minor_version)
    *minor_version = document->doc->getPDFMinorVersion();
}