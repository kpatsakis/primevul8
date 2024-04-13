poppler_layers_iter_get_title (PopplerLayersIter *iter)
{
  Layer *layer;
  
  g_return_val_if_fail (iter != nullptr, NULL);
  
  layer = (Layer *)g_list_nth_data (iter->items, iter->index);

  return layer->label ? g_strdup (layer->label) : nullptr;
}