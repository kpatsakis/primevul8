layer_free (Layer *layer)
{
  if (G_UNLIKELY (!layer))
    return;

  if (layer->kids) {
	  g_list_foreach (layer->kids, (GFunc)layer_free, nullptr);
	  g_list_free (layer->kids);
  }

  if (layer->label) {
	  g_free (layer->label);
  }

  g_slice_free (Layer, layer);
}