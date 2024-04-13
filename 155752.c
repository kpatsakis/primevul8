XML_MemMalloc(XML_Parser parser, size_t size) {
  if (parser == NULL)
    return NULL;
  return MALLOC(parser, size);
}