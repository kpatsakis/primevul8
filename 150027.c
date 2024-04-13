unset_addr_list_end(UnsetAddrList* uslist)
{
  if (IS_NOT_NULL(uslist->us))
    xfree(uslist->us);
}