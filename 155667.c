appendAttributeValue(XML_Parser parser, const ENCODING *enc, XML_Bool isCdata,
                     const char *ptr, const char *end, STRING_POOL *pool,
                     enum XML_Account account) {
  DTD *const dtd = parser->m_dtd; /* save one level of indirection */
#ifndef XML_DTD
  UNUSED_P(account);
#endif

  for (;;) {
    const char *next
        = ptr; /* XmlAttributeValueTok doesn't always set the last arg */
    int tok = XmlAttributeValueTok(enc, ptr, end, &next);
#ifdef XML_DTD
    if (! accountingDiffTolerated(parser, tok, ptr, next, __LINE__, account)) {
      accountingOnAbort(parser);
      return XML_ERROR_AMPLIFICATION_LIMIT_BREACH;
    }
#endif
    switch (tok) {
    case XML_TOK_NONE:
      return XML_ERROR_NONE;
    case XML_TOK_INVALID:
      if (enc == parser->m_encoding)
        parser->m_eventPtr = next;
      return XML_ERROR_INVALID_TOKEN;
    case XML_TOK_PARTIAL:
      if (enc == parser->m_encoding)
        parser->m_eventPtr = ptr;
      return XML_ERROR_INVALID_TOKEN;
    case XML_TOK_CHAR_REF: {
      XML_Char buf[XML_ENCODE_MAX];
      int i;
      int n = XmlCharRefNumber(enc, ptr);
      if (n < 0) {
        if (enc == parser->m_encoding)
          parser->m_eventPtr = ptr;
        return XML_ERROR_BAD_CHAR_REF;
      }
      if (! isCdata && n == 0x20 /* space */
          && (poolLength(pool) == 0 || poolLastChar(pool) == 0x20))
        break;
      n = XmlEncode(n, (ICHAR *)buf);
      /* The XmlEncode() functions can never return 0 here.  That
       * error return happens if the code point passed in is either
       * negative or greater than or equal to 0x110000.  The
       * XmlCharRefNumber() functions will all return a number
       * strictly less than 0x110000 or a negative value if an error
       * occurred.  The negative value is intercepted above, so
       * XmlEncode() is never passed a value it might return an
       * error for.
       */
      for (i = 0; i < n; i++) {
        if (! poolAppendChar(pool, buf[i]))
          return XML_ERROR_NO_MEMORY;
      }
    } break;
    case XML_TOK_DATA_CHARS:
      if (! poolAppend(pool, enc, ptr, next))
        return XML_ERROR_NO_MEMORY;
      break;
    case XML_TOK_TRAILING_CR:
      next = ptr + enc->minBytesPerChar;
      /* fall through */
    case XML_TOK_ATTRIBUTE_VALUE_S:
    case XML_TOK_DATA_NEWLINE:
      if (! isCdata && (poolLength(pool) == 0 || poolLastChar(pool) == 0x20))
        break;
      if (! poolAppendChar(pool, 0x20))
        return XML_ERROR_NO_MEMORY;
      break;
    case XML_TOK_ENTITY_REF: {
      const XML_Char *name;
      ENTITY *entity;
      char checkEntityDecl;
      XML_Char ch = (XML_Char)XmlPredefinedEntityName(
          enc, ptr + enc->minBytesPerChar, next - enc->minBytesPerChar);
      if (ch) {
#ifdef XML_DTD
        /* NOTE: We are replacing 4-6 characters original input for 1 character
         *       so there is no amplification and hence recording without
         *       protection. */
        accountingDiffTolerated(parser, tok, (char *)&ch,
                                ((char *)&ch) + sizeof(XML_Char), __LINE__,
                                XML_ACCOUNT_ENTITY_EXPANSION);
#endif /* XML_DTD */
        if (! poolAppendChar(pool, ch))
          return XML_ERROR_NO_MEMORY;
        break;
      }
      name = poolStoreString(&parser->m_temp2Pool, enc,
                             ptr + enc->minBytesPerChar,
                             next - enc->minBytesPerChar);
      if (! name)
        return XML_ERROR_NO_MEMORY;
      entity = (ENTITY *)lookup(parser, &dtd->generalEntities, name, 0);
      poolDiscard(&parser->m_temp2Pool);
      /* First, determine if a check for an existing declaration is needed;
         if yes, check that the entity exists, and that it is internal.
      */
      if (pool == &dtd->pool) /* are we called from prolog? */
        checkEntityDecl =
#ifdef XML_DTD
            parser->m_prologState.documentEntity &&
#endif /* XML_DTD */
            (dtd->standalone ? ! parser->m_openInternalEntities
                             : ! dtd->hasParamEntityRefs);
      else /* if (pool == &parser->m_tempPool): we are called from content */
        checkEntityDecl = ! dtd->hasParamEntityRefs || dtd->standalone;
      if (checkEntityDecl) {
        if (! entity)
          return XML_ERROR_UNDEFINED_ENTITY;
        else if (! entity->is_internal)
          return XML_ERROR_ENTITY_DECLARED_IN_PE;
      } else if (! entity) {
        /* Cannot report skipped entity here - see comments on
           parser->m_skippedEntityHandler.
        if (parser->m_skippedEntityHandler)
          parser->m_skippedEntityHandler(parser->m_handlerArg, name, 0);
        */
        /* Cannot call the default handler because this would be
           out of sync with the call to the startElementHandler.
        if ((pool == &parser->m_tempPool) && parser->m_defaultHandler)
          reportDefault(parser, enc, ptr, next);
        */
        break;
      }
      if (entity->open) {
        if (enc == parser->m_encoding) {
          /* It does not appear that this line can be executed.
           *
           * The "if (entity->open)" check catches recursive entity
           * definitions.  In order to be called with an open
           * entity, it must have gone through this code before and
           * been through the recursive call to
           * appendAttributeValue() some lines below.  That call
           * sets the local encoding ("enc") to the parser's
           * internal encoding (internal_utf8 or internal_utf16),
           * which can never be the same as the principle encoding.
           * It doesn't appear there is another code path that gets
           * here with entity->open being TRUE.
           *
           * Since it is not certain that this logic is watertight,
           * we keep the line and merely exclude it from coverage
           * tests.
           */
          parser->m_eventPtr = ptr; /* LCOV_EXCL_LINE */
        }
        return XML_ERROR_RECURSIVE_ENTITY_REF;
      }
      if (entity->notation) {
        if (enc == parser->m_encoding)
          parser->m_eventPtr = ptr;
        return XML_ERROR_BINARY_ENTITY_REF;
      }
      if (! entity->textPtr) {
        if (enc == parser->m_encoding)
          parser->m_eventPtr = ptr;
        return XML_ERROR_ATTRIBUTE_EXTERNAL_ENTITY_REF;
      } else {
        enum XML_Error result;
        const XML_Char *textEnd = entity->textPtr + entity->textLen;
        entity->open = XML_TRUE;
#ifdef XML_DTD
        entityTrackingOnOpen(parser, entity, __LINE__);
#endif
        result = appendAttributeValue(parser, parser->m_internalEncoding,
                                      isCdata, (const char *)entity->textPtr,
                                      (const char *)textEnd, pool,
                                      XML_ACCOUNT_ENTITY_EXPANSION);
#ifdef XML_DTD
        entityTrackingOnClose(parser, entity, __LINE__);
#endif
        entity->open = XML_FALSE;
        if (result)
          return result;
      }
    } break;
    default:
      /* The only token returned by XmlAttributeValueTok() that does
       * not have an explicit case here is XML_TOK_PARTIAL_CHAR.
       * Getting that would require an entity name to contain an
       * incomplete XML character (e.g. \xE2\x82); however previous
       * tokenisers will have already recognised and rejected such
       * names before XmlAttributeValueTok() gets a look-in.  This
       * default case should be retained as a safety net, but the code
       * excluded from coverage tests.
       *
       * LCOV_EXCL_START
       */
      if (enc == parser->m_encoding)
        parser->m_eventPtr = ptr;
      return XML_ERROR_UNEXPECTED_STATE;
      /* LCOV_EXCL_STOP */
    }
    ptr = next;
  }
  /* not reached */
}