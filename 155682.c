poolInit(STRING_POOL *pool, const XML_Memory_Handling_Suite *ms) {
  pool->blocks = NULL;
  pool->freeBlocks = NULL;
  pool->start = NULL;
  pool->ptr = NULL;
  pool->end = NULL;
  pool->mem = ms;
}