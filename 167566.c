get_optional_content_rbgroups (OCGs *ocg)
{
  Array *rb;
  GList *groups = nullptr;
  
  rb = ocg->getRBGroupsArray ();
  
  if (rb) {
    int i, j;

    for (i = 0; i < rb->getLength (); ++i) {
      Array *rb_array;
      GList *group = nullptr;

      Object obj = rb->get (i);
      if (!obj.isArray ()) {
	continue;
      }

      rb_array = obj.getArray ();
      for (j = 0; j < rb_array->getLength (); ++j) {
	OptionalContentGroup *oc;

        Object ref = rb_array->getNF (j);
	if (!ref.isRef ()) {
	  continue;
	}

	oc = ocg->findOcgByRef (ref.getRef ());
	group = g_list_prepend (group, oc);
      }

      groups = g_list_prepend (groups, group);
    }
  }

  return groups;
}