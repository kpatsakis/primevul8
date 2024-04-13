getContext(XML_Parser parser) {
  DTD *const dtd = parser->m_dtd; /* save one level of indirection */
  HASH_TABLE_ITER iter;
  XML_Bool needSep = XML_FALSE;

  if (dtd->defaultPrefix.binding) {
    int i;
    int len;
    if (! poolAppendChar(&parser->m_tempPool, XML_T(ASCII_EQUALS)))
      return NULL;
    len = dtd->defaultPrefix.binding->uriLen;
    if (parser->m_namespaceSeparator)
      len--;
    for (i = 0; i < len; i++) {
      if (! poolAppendChar(&parser->m_tempPool,
                           dtd->defaultPrefix.binding->uri[i])) {
        /* Because of memory caching, I don't believe this line can be
         * executed.
         *
         * This is part of a loop copying the default prefix binding
         * URI into the parser's temporary string pool.  Previously,
         * that URI was copied into the same string pool, with a
         * terminating NUL character, as part of setContext().  When
         * the pool was cleared, that leaves a block definitely big
         * enough to hold the URI on the free block list of the pool.
         * The URI copy in getContext() therefore cannot run out of
         * memory.
         *
         * If the pool is used between the setContext() and
         * getContext() calls, the worst it can do is leave a bigger
         * block on the front of the free list.  Given that this is
         * all somewhat inobvious and program logic can be changed, we
         * don't delete the line but we do exclude it from the test
         * coverage statistics.
         */
        return NULL; /* LCOV_EXCL_LINE */
      }
    }
    needSep = XML_TRUE;
  }

  hashTableIterInit(&iter, &(dtd->prefixes));
  for (;;) {
    int i;
    int len;
    const XML_Char *s;
    PREFIX *prefix = (PREFIX *)hashTableIterNext(&iter);
    if (! prefix)
      break;
    if (! prefix->binding) {
      /* This test appears to be (justifiable) paranoia.  There does
       * not seem to be a way of injecting a prefix without a binding
       * that doesn't get errored long before this function is called.
       * The test should remain for safety's sake, so we instead
       * exclude the following line from the coverage statistics.
       */
      continue; /* LCOV_EXCL_LINE */
    }
    if (needSep && ! poolAppendChar(&parser->m_tempPool, CONTEXT_SEP))
      return NULL;
    for (s = prefix->name; *s; s++)
      if (! poolAppendChar(&parser->m_tempPool, *s))
        return NULL;
    if (! poolAppendChar(&parser->m_tempPool, XML_T(ASCII_EQUALS)))
      return NULL;
    len = prefix->binding->uriLen;
    if (parser->m_namespaceSeparator)
      len--;
    for (i = 0; i < len; i++)
      if (! poolAppendChar(&parser->m_tempPool, prefix->binding->uri[i]))
        return NULL;
    needSep = XML_TRUE;
  }

  hashTableIterInit(&iter, &(dtd->generalEntities));
  for (;;) {
    const XML_Char *s;
    ENTITY *e = (ENTITY *)hashTableIterNext(&iter);
    if (! e)
      break;
    if (! e->open)
      continue;
    if (needSep && ! poolAppendChar(&parser->m_tempPool, CONTEXT_SEP))
      return NULL;
    for (s = e->name; *s; s++)
      if (! poolAppendChar(&parser->m_tempPool, *s))
        return 0;
    needSep = XML_TRUE;
  }

  if (! poolAppendChar(&parser->m_tempPool, XML_T('\0')))
    return NULL;
  return parser->m_tempPool.start;
}