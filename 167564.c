poppler_document_new_from_data (char        *data,
                                int          length,
                                const char  *password,
                                GError     **error)
{
  PDFDoc *newDoc;
  MemStream *str;
  GooString *password_g;

  if (!globalParams) {
    globalParams = new GlobalParams();
  }
  
  // create stream
  str = new MemStream(data, 0, length, Object(objNull));

  password_g = poppler_password_to_latin1(password);
  newDoc = new PDFDoc(str, password_g, password_g);
  delete password_g;

  return _poppler_document_new_from_pdfdoc (newDoc, error);
}