poppler_layers_iter_free (PopplerLayersIter *iter)
{
  if (G_UNLIKELY (iter == nullptr))
    return;

  g_object_unref (iter->document);
  g_slice_free (PopplerLayersIter, iter);
}