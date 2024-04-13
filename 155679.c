storeAtts(XML_Parser parser, const ENCODING *enc, const char *attStr,
          TAG_NAME *tagNamePtr, BINDING **bindingsPtr,
          enum XML_Account account) {
  DTD *const dtd = parser->m_dtd; /* save one level of indirection */
  ELEMENT_TYPE *elementType;
  int nDefaultAtts;
  const XML_Char **appAtts; /* the attribute list for the application */
  int attIndex = 0;
  int prefixLen;
  int i;
  int n;
  XML_Char *uri;
  int nPrefixes = 0;
  BINDING *binding;
  const XML_Char *localPart;

  /* lookup the element type name */
  elementType
      = (ELEMENT_TYPE *)lookup(parser, &dtd->elementTypes, tagNamePtr->str, 0);
  if (! elementType) {
    const XML_Char *name = poolCopyString(&dtd->pool, tagNamePtr->str);
    if (! name)
      return XML_ERROR_NO_MEMORY;
    elementType = (ELEMENT_TYPE *)lookup(parser, &dtd->elementTypes, name,
                                         sizeof(ELEMENT_TYPE));
    if (! elementType)
      return XML_ERROR_NO_MEMORY;
    if (parser->m_ns && ! setElementTypePrefix(parser, elementType))
      return XML_ERROR_NO_MEMORY;
  }
  nDefaultAtts = elementType->nDefaultAtts;

  /* get the attributes from the tokenizer */
  n = XmlGetAttributes(enc, attStr, parser->m_attsSize, parser->m_atts);

  /* Detect and prevent integer overflow */
  if (n > INT_MAX - nDefaultAtts) {
    return XML_ERROR_NO_MEMORY;
  }

  if (n + nDefaultAtts > parser->m_attsSize) {
    int oldAttsSize = parser->m_attsSize;
    ATTRIBUTE *temp;
#ifdef XML_ATTR_INFO
    XML_AttrInfo *temp2;
#endif

    /* Detect and prevent integer overflow */
    if ((nDefaultAtts > INT_MAX - INIT_ATTS_SIZE)
        || (n > INT_MAX - (nDefaultAtts + INIT_ATTS_SIZE))) {
      return XML_ERROR_NO_MEMORY;
    }

    parser->m_attsSize = n + nDefaultAtts + INIT_ATTS_SIZE;

    /* Detect and prevent integer overflow.
     * The preprocessor guard addresses the "always false" warning
     * from -Wtype-limits on platforms where
     * sizeof(unsigned int) < sizeof(size_t), e.g. on x86_64. */
#if UINT_MAX >= SIZE_MAX
    if ((unsigned)parser->m_attsSize > (size_t)(-1) / sizeof(ATTRIBUTE)) {
      parser->m_attsSize = oldAttsSize;
      return XML_ERROR_NO_MEMORY;
    }
#endif

    temp = (ATTRIBUTE *)REALLOC(parser, (void *)parser->m_atts,
                                parser->m_attsSize * sizeof(ATTRIBUTE));
    if (temp == NULL) {
      parser->m_attsSize = oldAttsSize;
      return XML_ERROR_NO_MEMORY;
    }
    parser->m_atts = temp;
#ifdef XML_ATTR_INFO
    /* Detect and prevent integer overflow.
     * The preprocessor guard addresses the "always false" warning
     * from -Wtype-limits on platforms where
     * sizeof(unsigned int) < sizeof(size_t), e.g. on x86_64. */
#  if UINT_MAX >= SIZE_MAX
    if ((unsigned)parser->m_attsSize > (size_t)(-1) / sizeof(XML_AttrInfo)) {
      parser->m_attsSize = oldAttsSize;
      return XML_ERROR_NO_MEMORY;
    }
#  endif

    temp2 = (XML_AttrInfo *)REALLOC(parser, (void *)parser->m_attInfo,
                                    parser->m_attsSize * sizeof(XML_AttrInfo));
    if (temp2 == NULL) {
      parser->m_attsSize = oldAttsSize;
      return XML_ERROR_NO_MEMORY;
    }
    parser->m_attInfo = temp2;
#endif
    if (n > oldAttsSize)
      XmlGetAttributes(enc, attStr, n, parser->m_atts);
  }

  appAtts = (const XML_Char **)parser->m_atts;
  for (i = 0; i < n; i++) {
    ATTRIBUTE *currAtt = &parser->m_atts[i];
#ifdef XML_ATTR_INFO
    XML_AttrInfo *currAttInfo = &parser->m_attInfo[i];
#endif
    /* add the name and value to the attribute list */
    ATTRIBUTE_ID *attId
        = getAttributeId(parser, enc, currAtt->name,
                         currAtt->name + XmlNameLength(enc, currAtt->name));
    if (! attId)
      return XML_ERROR_NO_MEMORY;
#ifdef XML_ATTR_INFO
    currAttInfo->nameStart
        = parser->m_parseEndByteIndex - (parser->m_parseEndPtr - currAtt->name);
    currAttInfo->nameEnd
        = currAttInfo->nameStart + XmlNameLength(enc, currAtt->name);
    currAttInfo->valueStart = parser->m_parseEndByteIndex
                              - (parser->m_parseEndPtr - currAtt->valuePtr);
    currAttInfo->valueEnd = parser->m_parseEndByteIndex
                            - (parser->m_parseEndPtr - currAtt->valueEnd);
#endif
    /* Detect duplicate attributes by their QNames. This does not work when
       namespace processing is turned on and different prefixes for the same
       namespace are used. For this case we have a check further down.
    */
    if ((attId->name)[-1]) {
      if (enc == parser->m_encoding)
        parser->m_eventPtr = parser->m_atts[i].name;
      return XML_ERROR_DUPLICATE_ATTRIBUTE;
    }
    (attId->name)[-1] = 1;
    appAtts[attIndex++] = attId->name;
    if (! parser->m_atts[i].normalized) {
      enum XML_Error result;
      XML_Bool isCdata = XML_TRUE;

      /* figure out whether declared as other than CDATA */
      if (attId->maybeTokenized) {
        int j;
        for (j = 0; j < nDefaultAtts; j++) {
          if (attId == elementType->defaultAtts[j].id) {
            isCdata = elementType->defaultAtts[j].isCdata;
            break;
          }
        }
      }

      /* normalize the attribute value */
      result = storeAttributeValue(
          parser, enc, isCdata, parser->m_atts[i].valuePtr,
          parser->m_atts[i].valueEnd, &parser->m_tempPool, account);
      if (result)
        return result;
      appAtts[attIndex] = poolStart(&parser->m_tempPool);
      poolFinish(&parser->m_tempPool);
    } else {
      /* the value did not need normalizing */
      appAtts[attIndex] = poolStoreString(&parser->m_tempPool, enc,
                                          parser->m_atts[i].valuePtr,
                                          parser->m_atts[i].valueEnd);
      if (appAtts[attIndex] == 0)
        return XML_ERROR_NO_MEMORY;
      poolFinish(&parser->m_tempPool);
    }
    /* handle prefixed attribute names */
    if (attId->prefix) {
      if (attId->xmlns) {
        /* deal with namespace declarations here */
        enum XML_Error result = addBinding(parser, attId->prefix, attId,
                                           appAtts[attIndex], bindingsPtr);
        if (result)
          return result;
        --attIndex;
      } else {
        /* deal with other prefixed names later */
        attIndex++;
        nPrefixes++;
        (attId->name)[-1] = 2;
      }
    } else
      attIndex++;
  }

  /* set-up for XML_GetSpecifiedAttributeCount and XML_GetIdAttributeIndex */
  parser->m_nSpecifiedAtts = attIndex;
  if (elementType->idAtt && (elementType->idAtt->name)[-1]) {
    for (i = 0; i < attIndex; i += 2)
      if (appAtts[i] == elementType->idAtt->name) {
        parser->m_idAttIndex = i;
        break;
      }
  } else
    parser->m_idAttIndex = -1;

  /* do attribute defaulting */
  for (i = 0; i < nDefaultAtts; i++) {
    const DEFAULT_ATTRIBUTE *da = elementType->defaultAtts + i;
    if (! (da->id->name)[-1] && da->value) {
      if (da->id->prefix) {
        if (da->id->xmlns) {
          enum XML_Error result = addBinding(parser, da->id->prefix, da->id,
                                             da->value, bindingsPtr);
          if (result)
            return result;
        } else {
          (da->id->name)[-1] = 2;
          nPrefixes++;
          appAtts[attIndex++] = da->id->name;
          appAtts[attIndex++] = da->value;
        }
      } else {
        (da->id->name)[-1] = 1;
        appAtts[attIndex++] = da->id->name;
        appAtts[attIndex++] = da->value;
      }
    }
  }
  appAtts[attIndex] = 0;

  /* expand prefixed attribute names, check for duplicates,
     and clear flags that say whether attributes were specified */
  i = 0;
  if (nPrefixes) {
    int j; /* hash table index */
    unsigned long version = parser->m_nsAttsVersion;

    /* Detect and prevent invalid shift */
    if (parser->m_nsAttsPower >= sizeof(unsigned int) * 8 /* bits per byte */) {
      return XML_ERROR_NO_MEMORY;
    }

    unsigned int nsAttsSize = 1u << parser->m_nsAttsPower;
    unsigned char oldNsAttsPower = parser->m_nsAttsPower;
    /* size of hash table must be at least 2 * (# of prefixed attributes) */
    if ((nPrefixes << 1)
        >> parser->m_nsAttsPower) { /* true for m_nsAttsPower = 0 */
      NS_ATT *temp;
      /* hash table size must also be a power of 2 and >= 8 */
      while (nPrefixes >> parser->m_nsAttsPower++)
        ;
      if (parser->m_nsAttsPower < 3)
        parser->m_nsAttsPower = 3;

      /* Detect and prevent invalid shift */
      if (parser->m_nsAttsPower >= sizeof(nsAttsSize) * 8 /* bits per byte */) {
        /* Restore actual size of memory in m_nsAtts */
        parser->m_nsAttsPower = oldNsAttsPower;
        return XML_ERROR_NO_MEMORY;
      }

      nsAttsSize = 1u << parser->m_nsAttsPower;

      /* Detect and prevent integer overflow.
       * The preprocessor guard addresses the "always false" warning
       * from -Wtype-limits on platforms where
       * sizeof(unsigned int) < sizeof(size_t), e.g. on x86_64. */
#if UINT_MAX >= SIZE_MAX
      if (nsAttsSize > (size_t)(-1) / sizeof(NS_ATT)) {
        /* Restore actual size of memory in m_nsAtts */
        parser->m_nsAttsPower = oldNsAttsPower;
        return XML_ERROR_NO_MEMORY;
      }
#endif

      temp = (NS_ATT *)REALLOC(parser, parser->m_nsAtts,
                               nsAttsSize * sizeof(NS_ATT));
      if (! temp) {
        /* Restore actual size of memory in m_nsAtts */
        parser->m_nsAttsPower = oldNsAttsPower;
        return XML_ERROR_NO_MEMORY;
      }
      parser->m_nsAtts = temp;
      version = 0; /* force re-initialization of m_nsAtts hash table */
    }
    /* using a version flag saves us from initializing m_nsAtts every time */
    if (! version) { /* initialize version flags when version wraps around */
      version = INIT_ATTS_VERSION;
      for (j = nsAttsSize; j != 0;)
        parser->m_nsAtts[--j].version = version;
    }
    parser->m_nsAttsVersion = --version;

    /* expand prefixed names and check for duplicates */
    for (; i < attIndex; i += 2) {
      const XML_Char *s = appAtts[i];
      if (s[-1] == 2) { /* prefixed */
        ATTRIBUTE_ID *id;
        const BINDING *b;
        unsigned long uriHash;
        struct siphash sip_state;
        struct sipkey sip_key;

        copy_salt_to_sipkey(parser, &sip_key);
        sip24_init(&sip_state, &sip_key);

        ((XML_Char *)s)[-1] = 0; /* clear flag */
        id = (ATTRIBUTE_ID *)lookup(parser, &dtd->attributeIds, s, 0);
        if (! id || ! id->prefix) {
          /* This code is walking through the appAtts array, dealing
           * with (in this case) a prefixed attribute name.  To be in
           * the array, the attribute must have already been bound, so
           * has to have passed through the hash table lookup once
           * already.  That implies that an entry for it already
           * exists, so the lookup above will return a pointer to
           * already allocated memory.  There is no opportunaity for
           * the allocator to fail, so the condition above cannot be
           * fulfilled.
           *
           * Since it is difficult to be certain that the above
           * analysis is complete, we retain the test and merely
           * remove the code from coverage tests.
           */
          return XML_ERROR_NO_MEMORY; /* LCOV_EXCL_LINE */
        }
        b = id->prefix->binding;
        if (! b)
          return XML_ERROR_UNBOUND_PREFIX;

        for (j = 0; j < b->uriLen; j++) {
          const XML_Char c = b->uri[j];
          if (! poolAppendChar(&parser->m_tempPool, c))
            return XML_ERROR_NO_MEMORY;
        }

        sip24_update(&sip_state, b->uri, b->uriLen * sizeof(XML_Char));

        while (*s++ != XML_T(ASCII_COLON))
          ;

        sip24_update(&sip_state, s, keylen(s) * sizeof(XML_Char));

        do { /* copies null terminator */
          if (! poolAppendChar(&parser->m_tempPool, *s))
            return XML_ERROR_NO_MEMORY;
        } while (*s++);

        uriHash = (unsigned long)sip24_final(&sip_state);

        { /* Check hash table for duplicate of expanded name (uriName).
             Derived from code in lookup(parser, HASH_TABLE *table, ...).
          */
          unsigned char step = 0;
          unsigned long mask = nsAttsSize - 1;
          j = uriHash & mask; /* index into hash table */
          while (parser->m_nsAtts[j].version == version) {
            /* for speed we compare stored hash values first */
            if (uriHash == parser->m_nsAtts[j].hash) {
              const XML_Char *s1 = poolStart(&parser->m_tempPool);
              const XML_Char *s2 = parser->m_nsAtts[j].uriName;
              /* s1 is null terminated, but not s2 */
              for (; *s1 == *s2 && *s1 != 0; s1++, s2++)
                ;
              if (*s1 == 0)
                return XML_ERROR_DUPLICATE_ATTRIBUTE;
            }
            if (! step)
              step = PROBE_STEP(uriHash, mask, parser->m_nsAttsPower);
            j < step ? (j += nsAttsSize - step) : (j -= step);
          }
        }

        if (parser->m_ns_triplets) { /* append namespace separator and prefix */
          parser->m_tempPool.ptr[-1] = parser->m_namespaceSeparator;
          s = b->prefix->name;
          do {
            if (! poolAppendChar(&parser->m_tempPool, *s))
              return XML_ERROR_NO_MEMORY;
          } while (*s++);
        }

        /* store expanded name in attribute list */
        s = poolStart(&parser->m_tempPool);
        poolFinish(&parser->m_tempPool);
        appAtts[i] = s;

        /* fill empty slot with new version, uriName and hash value */
        parser->m_nsAtts[j].version = version;
        parser->m_nsAtts[j].hash = uriHash;
        parser->m_nsAtts[j].uriName = s;

        if (! --nPrefixes) {
          i += 2;
          break;
        }
      } else                     /* not prefixed */
        ((XML_Char *)s)[-1] = 0; /* clear flag */
    }
  }
  /* clear flags for the remaining attributes */
  for (; i < attIndex; i += 2)
    ((XML_Char *)(appAtts[i]))[-1] = 0;
  for (binding = *bindingsPtr; binding; binding = binding->nextTagBinding)
    binding->attId->name[-1] = 0;

  if (! parser->m_ns)
    return XML_ERROR_NONE;

  /* expand the element type name */
  if (elementType->prefix) {
    binding = elementType->prefix->binding;
    if (! binding)
      return XML_ERROR_UNBOUND_PREFIX;
    localPart = tagNamePtr->str;
    while (*localPart++ != XML_T(ASCII_COLON))
      ;
  } else if (dtd->defaultPrefix.binding) {
    binding = dtd->defaultPrefix.binding;
    localPart = tagNamePtr->str;
  } else
    return XML_ERROR_NONE;
  prefixLen = 0;
  if (parser->m_ns_triplets && binding->prefix->name) {
    for (; binding->prefix->name[prefixLen++];)
      ; /* prefixLen includes null terminator */
  }
  tagNamePtr->localPart = localPart;
  tagNamePtr->uriLen = binding->uriLen;
  tagNamePtr->prefix = binding->prefix->name;
  tagNamePtr->prefixLen = prefixLen;
  for (i = 0; localPart[i++];)
    ; /* i includes null terminator */

  /* Detect and prevent integer overflow */
  if (binding->uriLen > INT_MAX - prefixLen
      || i > INT_MAX - (binding->uriLen + prefixLen)) {
    return XML_ERROR_NO_MEMORY;
  }

  n = i + binding->uriLen + prefixLen;
  if (n > binding->uriAlloc) {
    TAG *p;

    /* Detect and prevent integer overflow */
    if (n > INT_MAX - EXPAND_SPARE) {
      return XML_ERROR_NO_MEMORY;
    }
    /* Detect and prevent integer overflow.
     * The preprocessor guard addresses the "always false" warning
     * from -Wtype-limits on platforms where
     * sizeof(unsigned int) < sizeof(size_t), e.g. on x86_64. */
#if UINT_MAX >= SIZE_MAX
    if ((unsigned)(n + EXPAND_SPARE) > (size_t)(-1) / sizeof(XML_Char)) {
      return XML_ERROR_NO_MEMORY;
    }
#endif

    uri = (XML_Char *)MALLOC(parser, (n + EXPAND_SPARE) * sizeof(XML_Char));
    if (! uri)
      return XML_ERROR_NO_MEMORY;
    binding->uriAlloc = n + EXPAND_SPARE;
    memcpy(uri, binding->uri, binding->uriLen * sizeof(XML_Char));
    for (p = parser->m_tagStack; p; p = p->parent)
      if (p->name.str == binding->uri)
        p->name.str = uri;
    FREE(parser, binding->uri);
    binding->uri = uri;
  }
  /* if m_namespaceSeparator != '\0' then uri includes it already */
  uri = binding->uri + binding->uriLen;
  memcpy(uri, localPart, i * sizeof(XML_Char));
  /* we always have a namespace separator between localPart and prefix */
  if (prefixLen) {
    uri += i - 1;
    *uri = parser->m_namespaceSeparator; /* replace null terminator */
    memcpy(uri + 1, binding->prefix->name, prefixLen * sizeof(XML_Char));
  }
  tagNamePtr->str = binding->uri;
  return XML_ERROR_NONE;
}