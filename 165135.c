void *OsLibraryGetFunctionAddress(void *library,const char *functionName)
{
  if ((library == (void *) NULL) || (functionName == (const char *) NULL))
    return (void *) NULL;
#ifdef MAGICKCORE_WINDOWS_SUPPORT
    return (void *) GetProcAddress((HMODULE)library,functionName);
#else
    return (void *) dlsym(library,functionName);
#endif
}