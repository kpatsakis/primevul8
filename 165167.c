static MagickBooleanType LoadOpenCLLibrary(void)
{
  openCL_library=(MagickLibrary *) AcquireMagickMemory(sizeof(MagickLibrary));
  if (openCL_library == (MagickLibrary *) NULL)
    return(MagickFalse);

  if (BindOpenCLFunctions() == MagickFalse)
    {
      openCL_library=(MagickLibrary *)RelinquishMagickMemory(openCL_library);
      return(MagickFalse);
    }

  return(MagickTrue);
}