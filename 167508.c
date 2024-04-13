get_optional_content_items (OCGs *ocg)
{
  Array *order;
  GList *items = nullptr;
  
  order = ocg->getOrderArray ();

  if (order) {
    items = get_optional_content_items_sorted (ocg, nullptr, order);
  } else {
    const auto &ocgs = ocg->getOCGs ();

    for (const auto &oc : ocgs) {
      Layer *layer = layer_new (oc.second.get());

      items = g_list_prepend (items, layer);
    }
    
    items = g_list_reverse (items);
  }

  return items;
}