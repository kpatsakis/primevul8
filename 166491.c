static CURLcode file_disconnect(struct connectdata *conn,
                                bool dead_connection)
{
  struct FILEPROTO *file = conn->data->req.protop;
  (void)dead_connection; /* not used */

  if(file) {
    Curl_safefree(file->freepath);
    file->path = NULL;
    if(file->fd != -1)
      close(file->fd);
    file->fd = -1;
  }

  return CURLE_OK;
}