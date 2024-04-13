_poppler_document_get_layer_rbgroup (PopplerDocument *document,
				    Layer           *layer)
{
  GList *l;

  for (l = document->layers_rbgroups; l && l->data; l = g_list_next (l)) {
    GList *group = (GList *)l->data;

    if (g_list_find (group, layer->oc))
      return group;
  }

  return nullptr;
}