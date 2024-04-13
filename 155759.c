XML_MemFree(XML_Parser parser, void *ptr) {
  if (parser != NULL)
    FREE(parser, ptr);
}