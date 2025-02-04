static const char *GetOpenCLCacheDirectory()
{
  if (cache_directory == (char *) NULL)
    {
      if (cache_directory_lock == (SemaphoreInfo *) NULL)
        ActivateSemaphoreInfo(&cache_directory_lock);
      LockSemaphoreInfo(cache_directory_lock);
      if (cache_directory == (char *) NULL)
        {
          char
            *home,
            path[MagickPathExtent],
            *temp;

          MagickBooleanType
            status;

          struct stat
            attributes;

          temp=(char *) NULL;
          home=GetEnvironmentValue("MAGICK_OPENCL_CACHE_DIR");
          if (home == (char *) NULL)
            {
              home=GetEnvironmentValue("XDG_CACHE_HOME");
              if (home == (char *) NULL)
                home=GetEnvironmentValue("LOCALAPPDATA");
              if (home == (char *) NULL)
                home=GetEnvironmentValue("APPDATA");
              if (home == (char *) NULL)
                home=GetEnvironmentValue("USERPROFILE");
            }

          if (home != (char *) NULL)
            {
              /* first check if $HOME exists */
              (void) FormatLocaleString(path,MagickPathExtent,"%s",home);
              status=GetPathAttributes(path,&attributes);
              if (status == MagickFalse)
                status=MagickCreateDirectory(path);

              /* first check if $HOME/ImageMagick exists */
              if (status != MagickFalse)
                {
                  (void) FormatLocaleString(path,MagickPathExtent,
                    "%s%sImageMagick",home,DirectorySeparator);

                  status=GetPathAttributes(path,&attributes);
                  if (status == MagickFalse)
                    status=MagickCreateDirectory(path);
                }

              if (status != MagickFalse)
                {
                  temp=(char*) AcquireCriticalMemory(strlen(path)+1);
                  CopyMagickString(temp,path,strlen(path)+1);
                }
              home=DestroyString(home);
            }
          else
            {
              home=GetEnvironmentValue("HOME");
              if (home != (char *) NULL)
                {
                  /* first check if $HOME/.cache exists */
                  (void) FormatLocaleString(path,MagickPathExtent,"%s%s.cache",
                    home,DirectorySeparator);
                  status=GetPathAttributes(path,&attributes);
                  if (status == MagickFalse)
                    status=MagickCreateDirectory(path);

                  /* first check if $HOME/.cache/ImageMagick exists */
                  if (status != MagickFalse)
                    {
                      (void) FormatLocaleString(path,MagickPathExtent,
                        "%s%s.cache%sImageMagick",home,DirectorySeparator,
                        DirectorySeparator);
                      status=GetPathAttributes(path,&attributes);
                      if (status == MagickFalse)
                        status=MagickCreateDirectory(path);
                    }

                  if (status != MagickFalse)
                    {
                      temp=(char*) AcquireMagickMemory(strlen(path)+1);
                      CopyMagickString(temp,path,strlen(path)+1);
                    }
                  home=DestroyString(home);
                }
            }
          if (temp == (char *) NULL)
            temp=AcquireString("?");
          cache_directory=temp;
        }
      UnlockSemaphoreInfo(cache_directory_lock);
    }
  if (*cache_directory == '?')
    return((const char *) NULL);
  return(cache_directory);
}