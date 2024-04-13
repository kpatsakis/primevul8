poppler_document_new_from_file (const char  *uri,
				const char  *password,
				GError     **error)
{
  PDFDoc *newDoc;
  GooString *password_g;
  char *filename;

  if (!globalParams) {
    globalParams = new GlobalParams();
  }

  filename = g_filename_from_uri (uri, nullptr, error);
  if (!filename)
    return nullptr;

  password_g = poppler_password_to_latin1(password);

#ifdef G_OS_WIN32
  wchar_t *filenameW;
  int length;

  length = MultiByteToWideChar(CP_UTF8, 0, filename, -1, nullptr, 0);

  filenameW = new WCHAR[length];
  if (!filenameW)
      return NULL;

  length = MultiByteToWideChar(CP_UTF8, 0, filename, -1, filenameW, length);

  newDoc = new PDFDoc(filenameW, length, password_g, password_g);
  delete [] filenameW;
#else
  GooString *filename_g;
  filename_g = new GooString (filename);
  newDoc = new PDFDoc(filename_g, password_g, password_g);
#endif
  g_free (filename);

  delete password_g;

  return _poppler_document_new_from_pdfdoc (newDoc, error);
}