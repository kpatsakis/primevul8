build_model(XML_Parser parser) {
  /* Function build_model transforms the existing parser->m_dtd->scaffold
   * array of CONTENT_SCAFFOLD tree nodes into a new array of
   * XML_Content tree nodes followed by a gapless list of zero-terminated
   * strings. */
  DTD *const dtd = parser->m_dtd; /* save one level of indirection */
  XML_Content *ret;
  XML_Char *str; /* the current string writing location */

  /* Detect and prevent integer overflow.
   * The preprocessor guard addresses the "always false" warning
   * from -Wtype-limits on platforms where
   * sizeof(unsigned int) < sizeof(size_t), e.g. on x86_64. */
#if UINT_MAX >= SIZE_MAX
  if (dtd->scaffCount > (size_t)(-1) / sizeof(XML_Content)) {
    return NULL;
  }
  if (dtd->contentStringLen > (size_t)(-1) / sizeof(XML_Char)) {
    return NULL;
  }
#endif
  if (dtd->scaffCount * sizeof(XML_Content)
      > (size_t)(-1) - dtd->contentStringLen * sizeof(XML_Char)) {
    return NULL;
  }

  const size_t allocsize = (dtd->scaffCount * sizeof(XML_Content)
                            + (dtd->contentStringLen * sizeof(XML_Char)));

  ret = (XML_Content *)MALLOC(parser, allocsize);
  if (! ret)
    return NULL;

  /* What follows is an iterative implementation (of what was previously done
   * recursively in a dedicated function called "build_node".  The old recursive
   * build_node could be forced into stack exhaustion from input as small as a
   * few megabyte, and so that was a security issue.  Hence, a function call
   * stack is avoided now by resolving recursion.)
   *
   * The iterative approach works as follows:
   *
   * - We use space in the target array for building a temporary stack structure
   *   while that space is still unused.
   *   The stack grows from the array's end downwards and the "actual data"
   *   grows from the start upwards, sequentially.
   *   (Because stack grows downwards, pushing onto the stack is a decrement
   *   while popping off the stack is an increment.)
   *
   * - A stack element appears as a regular XML_Content node on the outside,
   *   but only uses a single field -- numchildren -- to store the source
   *   tree node array index.  These are the breadcrumbs leading the way back
   *   during pre-order (node first) depth-first traversal.
   *
   * - The reason we know the stack will never grow into (or overlap with)
   *   the area with data of value at the start of the array is because
   *   the overall number of elements to process matches the size of the array,
   *   and the sum of fully processed nodes and yet-to-be processed nodes
   *   on the stack, cannot be more than the total number of nodes.
   *   It is possible for the top of the stack and the about-to-write node
   *   to meet, but that is safe because we get the source index out
   *   before doing any writes on that node.
   */
  XML_Content *dest = ret; /* tree node writing location, moves upwards */
  XML_Content *const destLimit = &ret[dtd->scaffCount];
  XML_Content *const stackBottom = &ret[dtd->scaffCount];
  XML_Content *stackTop = stackBottom; /* i.e. stack is initially empty */
  str = (XML_Char *)&ret[dtd->scaffCount];

  /* Push source tree root node index onto the stack */
  (--stackTop)->numchildren = 0;

  for (; dest < destLimit; dest++) {
    /* Pop source tree node index off the stack */
    const int src_node = (int)(stackTop++)->numchildren;

    /* Convert item */
    dest->type = dtd->scaffold[src_node].type;
    dest->quant = dtd->scaffold[src_node].quant;
    if (dest->type == XML_CTYPE_NAME) {
      const XML_Char *src;
      dest->name = str;
      src = dtd->scaffold[src_node].name;
      for (;;) {
        *str++ = *src;
        if (! *src)
          break;
        src++;
      }
      dest->numchildren = 0;
      dest->children = NULL;
    } else {
      unsigned int i;
      int cn;
      dest->name = NULL;
      dest->numchildren = dtd->scaffold[src_node].childcnt;
      dest->children = &dest[1];

      /* Push children to the stack
       * in a way where the first child ends up at the top of the
       * (downwards growing) stack, in order to be processed first. */
      stackTop -= dest->numchildren;
      for (i = 0, cn = dtd->scaffold[src_node].firstchild;
           i < dest->numchildren; i++, cn = dtd->scaffold[cn].nextsib) {
        (stackTop + i)->numchildren = (unsigned int)cn;
      }
    }
  }

  return ret;
}