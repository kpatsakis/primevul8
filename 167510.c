poppler_document_new_from_stream (GInputStream *stream,
                                  goffset       length,
                                  const char   *password,
                                  GCancellable *cancellable,
                                  GError      **error)
{
  PDFDoc *newDoc;
  BaseStream *str;
  GooString *password_g;

  g_return_val_if_fail(G_IS_INPUT_STREAM(stream), NULL);
  g_return_val_if_fail(length == (goffset)-1 || length > 0, NULL);

  if (!globalParams) {
    globalParams = new GlobalParams();
  }

  if (!G_IS_SEEKABLE(stream) || !g_seekable_can_seek(G_SEEKABLE(stream))) {
    g_set_error_literal(error, G_IO_ERROR, G_IO_ERROR_NOT_SUPPORTED,
                        "Stream is not seekable");
    return nullptr;
  }

  if (stream_is_memory_buffer_or_local_file(stream)) {
    str = new PopplerInputStream(stream, cancellable, 0, gFalse, 0, Object(objNull));
  } else {
    CachedFile *cachedFile = new CachedFile(new PopplerCachedFileLoader(stream, cancellable, length), new GooString());
    str = new CachedFileStream(cachedFile, 0, gFalse, cachedFile->getLength(), Object(objNull));
  }

  password_g = poppler_password_to_latin1(password);
  newDoc = new PDFDoc(str, password_g, password_g);
  delete password_g;

  return _poppler_document_new_from_pdfdoc (newDoc, error);
}