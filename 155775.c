poolAppend(STRING_POOL *pool, const ENCODING *enc, const char *ptr,
           const char *end) {
  if (! pool->ptr && ! poolGrow(pool))
    return NULL;
  for (;;) {
    const enum XML_Convert_Result convert_res = XmlConvert(
        enc, &ptr, end, (ICHAR **)&(pool->ptr), (ICHAR *)pool->end);
    if ((convert_res == XML_CONVERT_COMPLETED)
        || (convert_res == XML_CONVERT_INPUT_INCOMPLETE))
      break;
    if (! poolGrow(pool))
      return NULL;
  }
  return pool->start;
}