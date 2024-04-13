void LibRaw::clearCancelFlag()
{
#ifdef _MSC_VER
  InterlockedExchange(&_exitflag, 0);
#else
  __sync_fetch_and_and(&_exitflag, 0);
#endif
#ifdef RAWSPEED_FASTEXIT
  if (_rawspeed_decoder)
  {
    RawSpeed::RawDecoder *d =
        static_cast<RawSpeed::RawDecoder *>(_rawspeed_decoder);
    d->resumeProcessing();
  }
#endif
}