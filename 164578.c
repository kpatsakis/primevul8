MagickPrivate void AnnotateComponentTerminus(void)
{
  if (annotate_semaphore == (SemaphoreInfo *) NULL)
    ActivateSemaphoreInfo(&annotate_semaphore);
  RelinquishSemaphoreInfo(&annotate_semaphore);
}