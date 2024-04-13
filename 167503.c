poppler_document_get_id (PopplerDocument *document,
			 gchar          **permanent_id,
			 gchar          **update_id)
{
  GooString permanent;
  GooString update;
  gboolean  retval = FALSE;

  g_return_val_if_fail (POPPLER_IS_DOCUMENT (document), FALSE);

  if (permanent_id)
    *permanent_id = nullptr;
  if (update_id)
    *update_id = nullptr;

  if (document->doc->getID (permanent_id ? &permanent : nullptr, update_id ? &update : nullptr)) {
    if (permanent_id)
      *permanent_id = (gchar *)g_memdup (permanent.getCString(), 32);
    if (update_id)
      *update_id = (gchar *)g_memdup (update.getCString(), 32);

    retval = TRUE;
  }

  return retval;
}