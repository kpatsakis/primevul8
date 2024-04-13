storeAttributeValue(XML_Parser parser, const ENCODING *enc, XML_Bool isCdata,
                    const char *ptr, const char *end, STRING_POOL *pool,
                    enum XML_Account account) {
  enum XML_Error result
      = appendAttributeValue(parser, enc, isCdata, ptr, end, pool, account);
  if (result)
    return result;
  if (! isCdata && poolLength(pool) && poolLastChar(pool) == 0x20)
    poolChop(pool);
  if (! poolAppendChar(pool, XML_T('\0')))
    return XML_ERROR_NO_MEMORY;
  return XML_ERROR_NONE;
}