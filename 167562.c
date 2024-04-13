poppler_layers_iter_new (PopplerDocument *document)
{
  PopplerLayersIter *iter;
  GList *items;

  items = _poppler_document_get_layers (document);

  if (!items)
    return nullptr;

  iter = g_slice_new0 (PopplerLayersIter);
  iter->document = (PopplerDocument *)g_object_ref (document);
  iter->items = items;

  return iter;
}