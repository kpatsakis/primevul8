setContext(XML_Parser parser, const XML_Char *context) {
  DTD *const dtd = parser->m_dtd; /* save one level of indirection */
  const XML_Char *s = context;

  while (*context != XML_T('\0')) {
    if (*s == CONTEXT_SEP || *s == XML_T('\0')) {
      ENTITY *e;
      if (! poolAppendChar(&parser->m_tempPool, XML_T('\0')))
        return XML_FALSE;
      e = (ENTITY *)lookup(parser, &dtd->generalEntities,
                           poolStart(&parser->m_tempPool), 0);
      if (e)
        e->open = XML_TRUE;
      if (*s != XML_T('\0'))
        s++;
      context = s;
      poolDiscard(&parser->m_tempPool);
    } else if (*s == XML_T(ASCII_EQUALS)) {
      PREFIX *prefix;
      if (poolLength(&parser->m_tempPool) == 0)
        prefix = &dtd->defaultPrefix;
      else {
        if (! poolAppendChar(&parser->m_tempPool, XML_T('\0')))
          return XML_FALSE;
        prefix
            = (PREFIX *)lookup(parser, &dtd->prefixes,
                               poolStart(&parser->m_tempPool), sizeof(PREFIX));
        if (! prefix)
          return XML_FALSE;
        if (prefix->name == poolStart(&parser->m_tempPool)) {
          prefix->name = poolCopyString(&dtd->pool, prefix->name);
          if (! prefix->name)
            return XML_FALSE;
        }
        poolDiscard(&parser->m_tempPool);
      }
      for (context = s + 1; *context != CONTEXT_SEP && *context != XML_T('\0');
           context++)
        if (! poolAppendChar(&parser->m_tempPool, *context))
          return XML_FALSE;
      if (! poolAppendChar(&parser->m_tempPool, XML_T('\0')))
        return XML_FALSE;
      if (addBinding(parser, prefix, NULL, poolStart(&parser->m_tempPool),
                     &parser->m_inheritedBindings)
          != XML_ERROR_NONE)
        return XML_FALSE;
      poolDiscard(&parser->m_tempPool);
      if (*context != XML_T('\0'))
        ++context;
      s = context;
    } else {
      if (! poolAppendChar(&parser->m_tempPool, *s))
        return XML_FALSE;
      s++;
    }
  }
  return XML_TRUE;
}