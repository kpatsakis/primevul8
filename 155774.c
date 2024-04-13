copyEntityTable(XML_Parser oldParser, HASH_TABLE *newTable,
                STRING_POOL *newPool, const HASH_TABLE *oldTable) {
  HASH_TABLE_ITER iter;
  const XML_Char *cachedOldBase = NULL;
  const XML_Char *cachedNewBase = NULL;

  hashTableIterInit(&iter, oldTable);

  for (;;) {
    ENTITY *newE;
    const XML_Char *name;
    const ENTITY *oldE = (ENTITY *)hashTableIterNext(&iter);
    if (! oldE)
      break;
    name = poolCopyString(newPool, oldE->name);
    if (! name)
      return 0;
    newE = (ENTITY *)lookup(oldParser, newTable, name, sizeof(ENTITY));
    if (! newE)
      return 0;
    if (oldE->systemId) {
      const XML_Char *tem = poolCopyString(newPool, oldE->systemId);
      if (! tem)
        return 0;
      newE->systemId = tem;
      if (oldE->base) {
        if (oldE->base == cachedOldBase)
          newE->base = cachedNewBase;
        else {
          cachedOldBase = oldE->base;
          tem = poolCopyString(newPool, cachedOldBase);
          if (! tem)
            return 0;
          cachedNewBase = newE->base = tem;
        }
      }
      if (oldE->publicId) {
        tem = poolCopyString(newPool, oldE->publicId);
        if (! tem)
          return 0;
        newE->publicId = tem;
      }
    } else {
      const XML_Char *tem
          = poolCopyStringN(newPool, oldE->textPtr, oldE->textLen);
      if (! tem)
        return 0;
      newE->textPtr = tem;
      newE->textLen = oldE->textLen;
    }
    if (oldE->notation) {
      const XML_Char *tem = poolCopyString(newPool, oldE->notation);
      if (! tem)
        return 0;
      newE->notation = tem;
    }
    newE->is_param = oldE->is_param;
    newE->is_internal = oldE->is_internal;
  }
  return 1;
}