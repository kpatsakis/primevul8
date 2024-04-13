getElementType(XML_Parser parser, const ENCODING *enc, const char *ptr,
               const char *end) {
  DTD *const dtd = parser->m_dtd; /* save one level of indirection */
  const XML_Char *name = poolStoreString(&dtd->pool, enc, ptr, end);
  ELEMENT_TYPE *ret;

  if (! name)
    return NULL;
  ret = (ELEMENT_TYPE *)lookup(parser, &dtd->elementTypes, name,
                               sizeof(ELEMENT_TYPE));
  if (! ret)
    return NULL;
  if (ret->name != name)
    poolDiscard(&dtd->pool);
  else {
    poolFinish(&dtd->pool);
    if (! setElementTypePrefix(parser, ret))
      return NULL;
  }
  return ret;
}