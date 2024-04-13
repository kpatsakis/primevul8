MagickExport void *MapBlob(int file,const MapMode mode,
  const MagickOffsetType offset,const size_t length)
{
#if defined(MAGICKCORE_HAVE_MMAP)
  int
    flags,
    protection;

  void
    *map;

  /*
    Map file.
  */
  flags=0;
  if (file == -1)
#if defined(MAP_ANONYMOUS)
    flags|=MAP_ANONYMOUS;
#else
    return(NULL);
#endif
  switch (mode)
  {
    case ReadMode:
    default:
    {
      protection=PROT_READ;
      flags|=MAP_PRIVATE;
      break;
    }
    case WriteMode:
    {
      protection=PROT_WRITE;
      flags|=MAP_SHARED;
      break;
    }
    case IOMode:
    {
      protection=PROT_READ | PROT_WRITE;
      flags|=MAP_SHARED;
      break;
    }
  }
#if !defined(MAGICKCORE_HAVE_HUGEPAGES) || !defined(MAP_HUGETLB)
  map=mmap((char *) NULL,length,protection,flags,file,offset);
#else
  map=mmap((char *) NULL,length,protection,flags | MAP_HUGETLB,file,offset);
  if (map == MAP_FAILED)
    map=mmap((char *) NULL,length,protection,flags,file,offset);
#endif
  if (map == MAP_FAILED)
    return(NULL);
  return(map);
#else
  (void) file;
  (void) mode;
  (void) offset;
  (void) length;
  return(NULL);
#endif
}