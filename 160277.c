void LibRaw::setCancelFlag()
{
#ifdef _MSC_VER
  InterlockedExchange(&_exitflag, 1);
#else
  __sync_fetch_and_add(&_exitflag, 1);
#endif
#ifdef RAWSPEED_FASTEXIT
  if (_rawspeed_decoder)
  {
    RawSpeed::RawDecoder *d =
        static_cast<RawSpeed::RawDecoder *>(_rawspeed_decoder);
    d->cancelProcessing();
  }
#endif
}