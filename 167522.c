poppler_layers_iter_copy (PopplerLayersIter *iter)
{
  PopplerLayersIter *new_iter;

  g_return_val_if_fail (iter != nullptr, NULL);
  
  new_iter = g_slice_dup (PopplerLayersIter, iter);
  new_iter->document = (PopplerDocument *) g_object_ref (new_iter->document);
  
  return new_iter;
}