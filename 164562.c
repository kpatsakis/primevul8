MagickPrivate MagickBooleanType AnnotateComponentGenesis(void)
{
  if (annotate_semaphore == (SemaphoreInfo *) NULL)
    annotate_semaphore=AcquireSemaphoreInfo();
  return(MagickTrue);
}