poppler_layers_iter_get_layer (PopplerLayersIter *iter)
{
  Layer *layer;
  PopplerLayer *poppler_layer = nullptr;
  
  g_return_val_if_fail (iter != nullptr, NULL);
  
  layer = (Layer *)g_list_nth_data (iter->items, iter->index);
  if (layer->oc) {
    GList *rb_group = nullptr;

    rb_group = _poppler_document_get_layer_rbgroup (iter->document, layer);
    poppler_layer = _poppler_layer_new (iter->document, layer, rb_group);
  }
  
  return poppler_layer;
}