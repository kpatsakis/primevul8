LibRaw_windows_datastream::LibRaw_windows_datastream(HANDLE hFile)
    : LibRaw_buffer_datastream(NULL, 0), hMap_(0), pView_(NULL)
{
  Open(hFile);
  reconstruct_base();
}