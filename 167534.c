poppler_document_save (PopplerDocument  *document,
		       const char       *uri,
		       GError          **error)
{
  char *filename;
  gboolean retval = FALSE;

  g_return_val_if_fail (POPPLER_IS_DOCUMENT (document), FALSE);

  filename = g_filename_from_uri (uri, nullptr, error);
  if (filename != nullptr) {
    GooString *fname = new GooString (filename);
    int err_code;
    g_free (filename);

    err_code = document->doc->saveAs (fname);
    retval = handle_save_error (err_code, error);
    delete fname;
  }

  return retval;
}