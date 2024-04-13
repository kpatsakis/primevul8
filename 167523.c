poppler_document_layers_free (PopplerDocument *document)
{
  if (G_UNLIKELY (!document->layers))
    return;

  g_list_foreach (document->layers, (GFunc)layer_free, nullptr);
  g_list_free (document->layers);

  g_list_foreach (document->layers_rbgroups, (GFunc)g_list_free, nullptr);
  g_list_free (document->layers_rbgroups);

  document->layers = nullptr;
  document->layers_rbgroups = nullptr;
}