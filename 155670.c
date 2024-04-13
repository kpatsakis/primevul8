nextScaffoldPart(XML_Parser parser) {
  DTD *const dtd = parser->m_dtd; /* save one level of indirection */
  CONTENT_SCAFFOLD *me;
  int next;

  if (! dtd->scaffIndex) {
    dtd->scaffIndex = (int *)MALLOC(parser, parser->m_groupSize * sizeof(int));
    if (! dtd->scaffIndex)
      return -1;
    dtd->scaffIndex[0] = 0;
  }

  if (dtd->scaffCount >= dtd->scaffSize) {
    CONTENT_SCAFFOLD *temp;
    if (dtd->scaffold) {
      /* Detect and prevent integer overflow */
      if (dtd->scaffSize > UINT_MAX / 2u) {
        return -1;
      }
      /* Detect and prevent integer overflow.
       * The preprocessor guard addresses the "always false" warning
       * from -Wtype-limits on platforms where
       * sizeof(unsigned int) < sizeof(size_t), e.g. on x86_64. */
#if UINT_MAX >= SIZE_MAX
      if (dtd->scaffSize > (size_t)(-1) / 2u / sizeof(CONTENT_SCAFFOLD)) {
        return -1;
      }
#endif

      temp = (CONTENT_SCAFFOLD *)REALLOC(
          parser, dtd->scaffold, dtd->scaffSize * 2 * sizeof(CONTENT_SCAFFOLD));
      if (temp == NULL)
        return -1;
      dtd->scaffSize *= 2;
    } else {
      temp = (CONTENT_SCAFFOLD *)MALLOC(parser, INIT_SCAFFOLD_ELEMENTS
                                                    * sizeof(CONTENT_SCAFFOLD));
      if (temp == NULL)
        return -1;
      dtd->scaffSize = INIT_SCAFFOLD_ELEMENTS;
    }
    dtd->scaffold = temp;
  }
  next = dtd->scaffCount++;
  me = &dtd->scaffold[next];
  if (dtd->scaffLevel) {
    CONTENT_SCAFFOLD *parent
        = &dtd->scaffold[dtd->scaffIndex[dtd->scaffLevel - 1]];
    if (parent->lastchild) {
      dtd->scaffold[parent->lastchild].nextsib = next;
    }
    if (! parent->childcnt)
      parent->firstchild = next;
    parent->lastchild = next;
    parent->childcnt++;
  }
  me->firstchild = me->lastchild = me->childcnt = me->nextsib = 0;
  return next;
}