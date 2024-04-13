poppler_layers_iter_get_child (PopplerLayersIter *parent)
{
  PopplerLayersIter *child;
  Layer *layer;

  g_return_val_if_fail (parent != nullptr, NULL);
	
  layer = (Layer *) g_list_nth_data (parent->items, parent->index);
  if (!layer || !layer->kids)
    return nullptr;

  child = g_slice_new0 (PopplerLayersIter);
  child->document = (PopplerDocument *)g_object_ref (parent->document);
  child->items = layer->kids;

  g_assert (child->items);

  return child;
}