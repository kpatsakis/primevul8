get_optional_content_items_sorted (OCGs *ocg, Layer *parent, Array *order)
{
  GList *items = nullptr;
  Layer *last_item = parent;
  int i;

  for (i = 0; i < order->getLength (); ++i) {
    Object orderItem = order->get (i);

    if (orderItem.isDict ()) {
      Object ref = order->getNF (i);
      if (ref.isRef ()) {
        OptionalContentGroup *oc = ocg->findOcgByRef (ref.getRef ());
	Layer *layer = layer_new (oc);

	items = g_list_prepend (items, layer);
	last_item = layer;
      }
    } else if (orderItem.isArray () && orderItem.arrayGetLength () > 0) {
      if (!last_item) {
        last_item = layer_new (nullptr);
	items = g_list_prepend (items, last_item);
      }
      last_item->kids = get_optional_content_items_sorted (ocg, last_item, orderItem.getArray ());
      last_item = nullptr;
    } else if (orderItem.isString ()) {
      last_item->label = _poppler_goo_string_to_utf8 (orderItem.getString ());
    }
  }
  
  return g_list_reverse (items);
}