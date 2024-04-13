poppler_layers_iter_next (PopplerLayersIter *iter)
{
  g_return_val_if_fail (iter != nullptr, FALSE);

  iter->index++;
  if (iter->index >= (gint)g_list_length (iter->items))
    return FALSE;
  
  return TRUE;
}