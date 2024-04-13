void LibRaw::checkCancel()
{
#ifdef _MSC_VER
  if (InterlockedExchange(&_exitflag, 0))
    throw LIBRAW_EXCEPTION_CANCELLED_BY_CALLBACK;
#else
  if (__sync_fetch_and_and(&_exitflag, 0))
    throw LIBRAW_EXCEPTION_CANCELLED_BY_CALLBACK;
#endif
}