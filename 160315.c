LibRaw_windows_datastream::~LibRaw_windows_datastream()
{
  if (pView_ != NULL)
    ::UnmapViewOfFile(pView_);

  if (hMap_ != 0)
    ::CloseHandle(hMap_);
}