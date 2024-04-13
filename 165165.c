static MagickBooleanType CanWriteProfileToFile(const char *filename)
{
  FILE
    *profileFile;

  profileFile=fopen(filename,"ab");

  if (profileFile == (FILE *)NULL)
    return(MagickFalse);

  fclose(profileFile);
  return(MagickTrue);
}