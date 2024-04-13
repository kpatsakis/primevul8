layer_new (OptionalContentGroup *oc)
{
  Layer *layer;

  layer = g_slice_new0 (Layer);
  layer->oc = oc;

  return layer;
}