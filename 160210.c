LibRaw_windows_datastream::LibRaw_windows_datastream(const TCHAR *sFile)
    : LibRaw_buffer_datastream(NULL, 0), hMap_(0), pView_(NULL)
{
  HANDLE hFile = CreateFile(sFile, GENERIC_READ, 0, 0, OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL, 0);
  if (hFile == INVALID_HANDLE_VALUE)
    throw std::runtime_error("failed to open the file");

  try
  {
    Open(hFile);
  }
  catch (...)
  {
    CloseHandle(hFile);
    throw;
  }

  CloseHandle(hFile); // windows will defer the actual closing of this handle
                      // until the hMap_ is closed
  reconstruct_base();
}