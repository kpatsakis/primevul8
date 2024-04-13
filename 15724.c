WandPrivate MagickBooleanType CLISimpleOperatorImages(MagickCLI *cli_wand,
  const char *option,const char *arg1,const char *arg2,ExceptionInfo *exception)
{
#if !USE_WAND_METHODS
  size_t
    n,
    i;
#endif

  assert(cli_wand != (MagickCLI *) NULL);
  assert(cli_wand->signature == MagickWandSignature);
  assert(cli_wand->wand.signature == MagickWandSignature);
  assert(cli_wand->wand.images != (Image *) NULL); /* images must be present */

  if (cli_wand->wand.debug != MagickFalse)
    (void) CLILogEvent(cli_wand,CommandEvent,GetMagickModule(),
         "- Simple Operator: %s \"%s\" \"%s\"", option,arg1,arg2);

#if !USE_WAND_METHODS
  /* FUTURE add appropriate tracing */
  i=0;
  n=GetImageListLength(cli_wand->wand.images);
  cli_wand->wand.images=GetFirstImageInList(cli_wand->wand.images);
  while (1) {
    i++;
    CLISimpleOperatorImage(cli_wand, option, arg1, arg2,exception);
    if ( cli_wand->wand.images->next == (Image *) NULL )
      break;
    cli_wand->wand.images=cli_wand->wand.images->next;
  }
  assert( i == n );
  cli_wand->wand.images=GetFirstImageInList(cli_wand->wand.images);
#else
  MagickResetIterator(&cli_wand->wand);
  while (MagickNextImage(&cli_wand->wand) != MagickFalse)
    (void) CLISimpleOperatorImage(cli_wand, option, arg1, arg2,exception);
  MagickResetIterator(&cli_wand->wand);
#endif
  return(MagickTrue);
}