poppler_document_new_from_gfile (GFile        *file,
                                 const char   *password,
                                 GCancellable *cancellable,
                                 GError      **error)
{
  PopplerDocument *document;
  GFileInputStream *stream;

  g_return_val_if_fail(G_IS_FILE(file), NULL);

  if (g_file_is_native(file)) {
    gchar *uri;

    uri = g_file_get_uri(file);
    document = poppler_document_new_from_file(uri, password, error);
    g_free(uri);

    return document;
  }

  stream = g_file_read(file, cancellable, error);
  if (!stream)
    return nullptr;

  document = poppler_document_new_from_stream(G_INPUT_STREAM(stream), -1, password, cancellable, error);
  g_object_unref(stream);

  return document;
}