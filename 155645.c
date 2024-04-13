doContent(XML_Parser parser, int startTagLevel, const ENCODING *enc,
          const char *s, const char *end, const char **nextPtr,
          XML_Bool haveMore, enum XML_Account account) {
  /* save one level of indirection */
  DTD *const dtd = parser->m_dtd;

  const char **eventPP;
  const char **eventEndPP;
  if (enc == parser->m_encoding) {
    eventPP = &parser->m_eventPtr;
    eventEndPP = &parser->m_eventEndPtr;
  } else {
    eventPP = &(parser->m_openInternalEntities->internalEventPtr);
    eventEndPP = &(parser->m_openInternalEntities->internalEventEndPtr);
  }
  *eventPP = s;

  for (;;) {
    const char *next = s; /* XmlContentTok doesn't always set the last arg */
    int tok = XmlContentTok(enc, s, end, &next);
#ifdef XML_DTD
    const char *accountAfter
        = ((tok == XML_TOK_TRAILING_RSQB) || (tok == XML_TOK_TRAILING_CR))
              ? (haveMore ? s /* i.e. 0 bytes */ : end)
              : next;
    if (! accountingDiffTolerated(parser, tok, s, accountAfter, __LINE__,
                                  account)) {
      accountingOnAbort(parser);
      return XML_ERROR_AMPLIFICATION_LIMIT_BREACH;
    }
#endif
    *eventEndPP = next;
    switch (tok) {
    case XML_TOK_TRAILING_CR:
      if (haveMore) {
        *nextPtr = s;
        return XML_ERROR_NONE;
      }
      *eventEndPP = end;
      if (parser->m_characterDataHandler) {
        XML_Char c = 0xA;
        parser->m_characterDataHandler(parser->m_handlerArg, &c, 1);
      } else if (parser->m_defaultHandler)
        reportDefault(parser, enc, s, end);
      /* We are at the end of the final buffer, should we check for
         XML_SUSPENDED, XML_FINISHED?
      */
      if (startTagLevel == 0)
        return XML_ERROR_NO_ELEMENTS;
      if (parser->m_tagLevel != startTagLevel)
        return XML_ERROR_ASYNC_ENTITY;
      *nextPtr = end;
      return XML_ERROR_NONE;
    case XML_TOK_NONE:
      if (haveMore) {
        *nextPtr = s;
        return XML_ERROR_NONE;
      }
      if (startTagLevel > 0) {
        if (parser->m_tagLevel != startTagLevel)
          return XML_ERROR_ASYNC_ENTITY;
        *nextPtr = s;
        return XML_ERROR_NONE;
      }
      return XML_ERROR_NO_ELEMENTS;
    case XML_TOK_INVALID:
      *eventPP = next;
      return XML_ERROR_INVALID_TOKEN;
    case XML_TOK_PARTIAL:
      if (haveMore) {
        *nextPtr = s;
        return XML_ERROR_NONE;
      }
      return XML_ERROR_UNCLOSED_TOKEN;
    case XML_TOK_PARTIAL_CHAR:
      if (haveMore) {
        *nextPtr = s;
        return XML_ERROR_NONE;
      }
      return XML_ERROR_PARTIAL_CHAR;
    case XML_TOK_ENTITY_REF: {
      const XML_Char *name;
      ENTITY *entity;
      XML_Char ch = (XML_Char)XmlPredefinedEntityName(
          enc, s + enc->minBytesPerChar, next - enc->minBytesPerChar);
      if (ch) {
#ifdef XML_DTD
        /* NOTE: We are replacing 4-6 characters original input for 1 character
         *       so there is no amplification and hence recording without
         *       protection. */
        accountingDiffTolerated(parser, tok, (char *)&ch,
                                ((char *)&ch) + sizeof(XML_Char), __LINE__,
                                XML_ACCOUNT_ENTITY_EXPANSION);
#endif /* XML_DTD */
        if (parser->m_characterDataHandler)
          parser->m_characterDataHandler(parser->m_handlerArg, &ch, 1);
        else if (parser->m_defaultHandler)
          reportDefault(parser, enc, s, next);
        break;
      }
      name = poolStoreString(&dtd->pool, enc, s + enc->minBytesPerChar,
                             next - enc->minBytesPerChar);
      if (! name)
        return XML_ERROR_NO_MEMORY;
      entity = (ENTITY *)lookup(parser, &dtd->generalEntities, name, 0);
      poolDiscard(&dtd->pool);
      /* First, determine if a check for an existing declaration is needed;
         if yes, check that the entity exists, and that it is internal,
         otherwise call the skipped entity or default handler.
      */
      if (! dtd->hasParamEntityRefs || dtd->standalone) {
        if (! entity)
          return XML_ERROR_UNDEFINED_ENTITY;
        else if (! entity->is_internal)
          return XML_ERROR_ENTITY_DECLARED_IN_PE;
      } else if (! entity) {
        if (parser->m_skippedEntityHandler)
          parser->m_skippedEntityHandler(parser->m_handlerArg, name, 0);
        else if (parser->m_defaultHandler)
          reportDefault(parser, enc, s, next);
        break;
      }
      if (entity->open)
        return XML_ERROR_RECURSIVE_ENTITY_REF;
      if (entity->notation)
        return XML_ERROR_BINARY_ENTITY_REF;
      if (entity->textPtr) {
        enum XML_Error result;
        if (! parser->m_defaultExpandInternalEntities) {
          if (parser->m_skippedEntityHandler)
            parser->m_skippedEntityHandler(parser->m_handlerArg, entity->name,
                                           0);
          else if (parser->m_defaultHandler)
            reportDefault(parser, enc, s, next);
          break;
        }
        result = processInternalEntity(parser, entity, XML_FALSE);
        if (result != XML_ERROR_NONE)
          return result;
      } else if (parser->m_externalEntityRefHandler) {
        const XML_Char *context;
        entity->open = XML_TRUE;
        context = getContext(parser);
        entity->open = XML_FALSE;
        if (! context)
          return XML_ERROR_NO_MEMORY;
        if (! parser->m_externalEntityRefHandler(
                parser->m_externalEntityRefHandlerArg, context, entity->base,
                entity->systemId, entity->publicId))
          return XML_ERROR_EXTERNAL_ENTITY_HANDLING;
        poolDiscard(&parser->m_tempPool);
      } else if (parser->m_defaultHandler)
        reportDefault(parser, enc, s, next);
      break;
    }
    case XML_TOK_START_TAG_NO_ATTS:
      /* fall through */
    case XML_TOK_START_TAG_WITH_ATTS: {
      TAG *tag;
      enum XML_Error result;
      XML_Char *toPtr;
      if (parser->m_freeTagList) {
        tag = parser->m_freeTagList;
        parser->m_freeTagList = parser->m_freeTagList->parent;
      } else {
        tag = (TAG *)MALLOC(parser, sizeof(TAG));
        if (! tag)
          return XML_ERROR_NO_MEMORY;
        tag->buf = (char *)MALLOC(parser, INIT_TAG_BUF_SIZE);
        if (! tag->buf) {
          FREE(parser, tag);
          return XML_ERROR_NO_MEMORY;
        }
        tag->bufEnd = tag->buf + INIT_TAG_BUF_SIZE;
      }
      tag->bindings = NULL;
      tag->parent = parser->m_tagStack;
      parser->m_tagStack = tag;
      tag->name.localPart = NULL;
      tag->name.prefix = NULL;
      tag->rawName = s + enc->minBytesPerChar;
      tag->rawNameLength = XmlNameLength(enc, tag->rawName);
      ++parser->m_tagLevel;
      {
        const char *rawNameEnd = tag->rawName + tag->rawNameLength;
        const char *fromPtr = tag->rawName;
        toPtr = (XML_Char *)tag->buf;
        for (;;) {
          int bufSize;
          int convLen;
          const enum XML_Convert_Result convert_res
              = XmlConvert(enc, &fromPtr, rawNameEnd, (ICHAR **)&toPtr,
                           (ICHAR *)tag->bufEnd - 1);
          convLen = (int)(toPtr - (XML_Char *)tag->buf);
          if ((fromPtr >= rawNameEnd)
              || (convert_res == XML_CONVERT_INPUT_INCOMPLETE)) {
            tag->name.strLen = convLen;
            break;
          }
          bufSize = (int)(tag->bufEnd - tag->buf) << 1;
          {
            char *temp = (char *)REALLOC(parser, tag->buf, bufSize);
            if (temp == NULL)
              return XML_ERROR_NO_MEMORY;
            tag->buf = temp;
            tag->bufEnd = temp + bufSize;
            toPtr = (XML_Char *)temp + convLen;
          }
        }
      }
      tag->name.str = (XML_Char *)tag->buf;
      *toPtr = XML_T('\0');
      result
          = storeAtts(parser, enc, s, &(tag->name), &(tag->bindings), account);
      if (result)
        return result;
      if (parser->m_startElementHandler)
        parser->m_startElementHandler(parser->m_handlerArg, tag->name.str,
                                      (const XML_Char **)parser->m_atts);
      else if (parser->m_defaultHandler)
        reportDefault(parser, enc, s, next);
      poolClear(&parser->m_tempPool);
      break;
    }
    case XML_TOK_EMPTY_ELEMENT_NO_ATTS:
      /* fall through */
    case XML_TOK_EMPTY_ELEMENT_WITH_ATTS: {
      const char *rawName = s + enc->minBytesPerChar;
      enum XML_Error result;
      BINDING *bindings = NULL;
      XML_Bool noElmHandlers = XML_TRUE;
      TAG_NAME name;
      name.str = poolStoreString(&parser->m_tempPool, enc, rawName,
                                 rawName + XmlNameLength(enc, rawName));
      if (! name.str)
        return XML_ERROR_NO_MEMORY;
      poolFinish(&parser->m_tempPool);
      result = storeAtts(parser, enc, s, &name, &bindings,
                         XML_ACCOUNT_NONE /* token spans whole start tag */);
      if (result != XML_ERROR_NONE) {
        freeBindings(parser, bindings);
        return result;
      }
      poolFinish(&parser->m_tempPool);
      if (parser->m_startElementHandler) {
        parser->m_startElementHandler(parser->m_handlerArg, name.str,
                                      (const XML_Char **)parser->m_atts);
        noElmHandlers = XML_FALSE;
      }
      if (parser->m_endElementHandler) {
        if (parser->m_startElementHandler)
          *eventPP = *eventEndPP;
        parser->m_endElementHandler(parser->m_handlerArg, name.str);
        noElmHandlers = XML_FALSE;
      }
      if (noElmHandlers && parser->m_defaultHandler)
        reportDefault(parser, enc, s, next);
      poolClear(&parser->m_tempPool);
      freeBindings(parser, bindings);
    }
      if ((parser->m_tagLevel == 0)
          && (parser->m_parsingStatus.parsing != XML_FINISHED)) {
        if (parser->m_parsingStatus.parsing == XML_SUSPENDED)
          parser->m_processor = epilogProcessor;
        else
          return epilogProcessor(parser, next, end, nextPtr);
      }
      break;
    case XML_TOK_END_TAG:
      if (parser->m_tagLevel == startTagLevel)
        return XML_ERROR_ASYNC_ENTITY;
      else {
        int len;
        const char *rawName;
        TAG *tag = parser->m_tagStack;
        parser->m_tagStack = tag->parent;
        tag->parent = parser->m_freeTagList;
        parser->m_freeTagList = tag;
        rawName = s + enc->minBytesPerChar * 2;
        len = XmlNameLength(enc, rawName);
        if (len != tag->rawNameLength
            || memcmp(tag->rawName, rawName, len) != 0) {
          *eventPP = rawName;
          return XML_ERROR_TAG_MISMATCH;
        }
        --parser->m_tagLevel;
        if (parser->m_endElementHandler) {
          const XML_Char *localPart;
          const XML_Char *prefix;
          XML_Char *uri;
          localPart = tag->name.localPart;
          if (parser->m_ns && localPart) {
            /* localPart and prefix may have been overwritten in
               tag->name.str, since this points to the binding->uri
               buffer which gets re-used; so we have to add them again
            */
            uri = (XML_Char *)tag->name.str + tag->name.uriLen;
            /* don't need to check for space - already done in storeAtts() */
            while (*localPart)
              *uri++ = *localPart++;
            prefix = (XML_Char *)tag->name.prefix;
            if (parser->m_ns_triplets && prefix) {
              *uri++ = parser->m_namespaceSeparator;
              while (*prefix)
                *uri++ = *prefix++;
            }
            *uri = XML_T('\0');
          }
          parser->m_endElementHandler(parser->m_handlerArg, tag->name.str);
        } else if (parser->m_defaultHandler)
          reportDefault(parser, enc, s, next);
        while (tag->bindings) {
          BINDING *b = tag->bindings;
          if (parser->m_endNamespaceDeclHandler)
            parser->m_endNamespaceDeclHandler(parser->m_handlerArg,
                                              b->prefix->name);
          tag->bindings = tag->bindings->nextTagBinding;
          b->nextTagBinding = parser->m_freeBindingList;
          parser->m_freeBindingList = b;
          b->prefix->binding = b->prevPrefixBinding;
        }
        if ((parser->m_tagLevel == 0)
            && (parser->m_parsingStatus.parsing != XML_FINISHED)) {
          if (parser->m_parsingStatus.parsing == XML_SUSPENDED)
            parser->m_processor = epilogProcessor;
          else
            return epilogProcessor(parser, next, end, nextPtr);
        }
      }
      break;
    case XML_TOK_CHAR_REF: {
      int n = XmlCharRefNumber(enc, s);
      if (n < 0)
        return XML_ERROR_BAD_CHAR_REF;
      if (parser->m_characterDataHandler) {
        XML_Char buf[XML_ENCODE_MAX];
        parser->m_characterDataHandler(parser->m_handlerArg, buf,
                                       XmlEncode(n, (ICHAR *)buf));
      } else if (parser->m_defaultHandler)
        reportDefault(parser, enc, s, next);
    } break;
    case XML_TOK_XML_DECL:
      return XML_ERROR_MISPLACED_XML_PI;
    case XML_TOK_DATA_NEWLINE:
      if (parser->m_characterDataHandler) {
        XML_Char c = 0xA;
        parser->m_characterDataHandler(parser->m_handlerArg, &c, 1);
      } else if (parser->m_defaultHandler)
        reportDefault(parser, enc, s, next);
      break;
    case XML_TOK_CDATA_SECT_OPEN: {
      enum XML_Error result;
      if (parser->m_startCdataSectionHandler)
        parser->m_startCdataSectionHandler(parser->m_handlerArg);
      /* BEGIN disabled code */
      /* Suppose you doing a transformation on a document that involves
         changing only the character data.  You set up a defaultHandler
         and a characterDataHandler.  The defaultHandler simply copies
         characters through.  The characterDataHandler does the
         transformation and writes the characters out escaping them as
         necessary.  This case will fail to work if we leave out the
         following two lines (because & and < inside CDATA sections will
         be incorrectly escaped).

         However, now we have a start/endCdataSectionHandler, so it seems
         easier to let the user deal with this.
      */
      else if (0 && parser->m_characterDataHandler)
        parser->m_characterDataHandler(parser->m_handlerArg, parser->m_dataBuf,
                                       0);
      /* END disabled code */
      else if (parser->m_defaultHandler)
        reportDefault(parser, enc, s, next);
      result
          = doCdataSection(parser, enc, &next, end, nextPtr, haveMore, account);
      if (result != XML_ERROR_NONE)
        return result;
      else if (! next) {
        parser->m_processor = cdataSectionProcessor;
        return result;
      }
    } break;
    case XML_TOK_TRAILING_RSQB:
      if (haveMore) {
        *nextPtr = s;
        return XML_ERROR_NONE;
      }
      if (parser->m_characterDataHandler) {
        if (MUST_CONVERT(enc, s)) {
          ICHAR *dataPtr = (ICHAR *)parser->m_dataBuf;
          XmlConvert(enc, &s, end, &dataPtr, (ICHAR *)parser->m_dataBufEnd);
          parser->m_characterDataHandler(
              parser->m_handlerArg, parser->m_dataBuf,
              (int)(dataPtr - (ICHAR *)parser->m_dataBuf));
        } else
          parser->m_characterDataHandler(
              parser->m_handlerArg, (XML_Char *)s,
              (int)((XML_Char *)end - (XML_Char *)s));
      } else if (parser->m_defaultHandler)
        reportDefault(parser, enc, s, end);
      /* We are at the end of the final buffer, should we check for
         XML_SUSPENDED, XML_FINISHED?
      */
      if (startTagLevel == 0) {
        *eventPP = end;
        return XML_ERROR_NO_ELEMENTS;
      }
      if (parser->m_tagLevel != startTagLevel) {
        *eventPP = end;
        return XML_ERROR_ASYNC_ENTITY;
      }
      *nextPtr = end;
      return XML_ERROR_NONE;
    case XML_TOK_DATA_CHARS: {
      XML_CharacterDataHandler charDataHandler = parser->m_characterDataHandler;
      if (charDataHandler) {
        if (MUST_CONVERT(enc, s)) {
          for (;;) {
            ICHAR *dataPtr = (ICHAR *)parser->m_dataBuf;
            const enum XML_Convert_Result convert_res = XmlConvert(
                enc, &s, next, &dataPtr, (ICHAR *)parser->m_dataBufEnd);
            *eventEndPP = s;
            charDataHandler(parser->m_handlerArg, parser->m_dataBuf,
                            (int)(dataPtr - (ICHAR *)parser->m_dataBuf));
            if ((convert_res == XML_CONVERT_COMPLETED)
                || (convert_res == XML_CONVERT_INPUT_INCOMPLETE))
              break;
            *eventPP = s;
          }
        } else
          charDataHandler(parser->m_handlerArg, (XML_Char *)s,
                          (int)((XML_Char *)next - (XML_Char *)s));
      } else if (parser->m_defaultHandler)
        reportDefault(parser, enc, s, next);
    } break;
    case XML_TOK_PI:
      if (! reportProcessingInstruction(parser, enc, s, next))
        return XML_ERROR_NO_MEMORY;
      break;
    case XML_TOK_COMMENT:
      if (! reportComment(parser, enc, s, next))
        return XML_ERROR_NO_MEMORY;
      break;
    default:
      /* All of the tokens produced by XmlContentTok() have their own
       * explicit cases, so this default is not strictly necessary.
       * However it is a useful safety net, so we retain the code and
       * simply exclude it from the coverage tests.
       *
       * LCOV_EXCL_START
       */
      if (parser->m_defaultHandler)
        reportDefault(parser, enc, s, next);
      break;
      /* LCOV_EXCL_STOP */
    }
    *eventPP = s = next;
    switch (parser->m_parsingStatus.parsing) {
    case XML_SUSPENDED:
      *nextPtr = next;
      return XML_ERROR_NONE;
    case XML_FINISHED:
      return XML_ERROR_ABORTED;
    default:;
    }
  }
  /* not reached */
}