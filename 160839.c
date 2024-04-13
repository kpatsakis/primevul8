getfnc_gather_random (void))(void (*)(const void*, size_t, int), int,
                        size_t, int)
{
#ifdef USE_ALL_RANDOM_MODULES
  static int (*fnc)(void (*)(const void*, size_t, int), int, size_t, int);

  if (fnc)
    return fnc;
# ifdef USE_RNDLINUX
  if ( !access (NAME_OF_DEV_RANDOM, R_OK)
       && !access (NAME_OF_DEV_URANDOM, R_OK))
    {
      fnc = rndlinux_gather_random;
      return fnc;
    }
# endif
# ifdef USE_RNDEGD
  if ( rndegd_connect_socket (1) != -1 )
    {
      fnc = rndegd_gather_random;
      return fnc;
    }
# endif
# ifdef USE_RNDUNIX
  fnc = rndunix_gather_random;
  return fnc;
# endif

  log_fatal (_("no entropy gathering module detected\n"));

#else
# ifdef USE_RNDLINUX
  return rndlinux_gather_random;
# endif
# ifdef USE_RNDUNIX
  return rndunix_gather_random;
# endif
# ifdef USE_RNDVMS
  return rndvms_gather_random;
# endif
# ifdef USE_RNDEGD
  return rndegd_gather_random;
# endif
# ifdef USE_RNDW32
  return rndw32_gather_random;
# endif
# ifdef USE_RNDRISCOS
  return rndriscos_gather_random;
# endif
#endif
  return NULL;
}