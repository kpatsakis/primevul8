i_free_callout_name_entry(st_callout_name_key* key, CalloutNameEntry* e,
                          void* arg ARG_UNUSED)
{
  xfree(e->name);
  /*xfree(key->s); */ /* is same as e->name */
  xfree(key);
  xfree(e);
  return ST_DELETE;
}