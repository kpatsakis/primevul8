static void CacheSignalHandler(int status)
{
  ThrowFatalException(CacheFatalError,"UnableToExtendPixelCache");
}