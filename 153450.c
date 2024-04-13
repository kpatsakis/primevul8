i_callout_callout_list_set(UChar* key, CalloutTagVal e, void* arg)
{
  int num;
  RegexExt* ext = (RegexExt* )arg;

  num = (int )e - 1;
  ext->callout_list[num].flag |= CALLOUT_TAG_LIST_FLAG_TAG_EXIST;
  return ST_CONTINUE;
}