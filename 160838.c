getfnc_fast_random_poll (void))( void (*)(const void*, size_t, int), int)
{
#ifdef USE_RNDW32
  return rndw32_gather_random_fast;
#endif
  return NULL;
}