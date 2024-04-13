unset_addr_list_add(UnsetAddrList* uslist, int offset, struct _Node* node)
{
  UnsetAddr* p;
  int size;

  if (uslist->num >= uslist->alloc) {
    size = uslist->alloc * 2;
    p = (UnsetAddr* )xrealloc(uslist->us, sizeof(UnsetAddr) * size);
    CHECK_NULL_RETURN_MEMERR(p);
    uslist->alloc = size;
    uslist->us    = p;
  }

  uslist->us[uslist->num].offset = offset;
  uslist->us[uslist->num].target = node;
  uslist->num++;
  return 0;
}