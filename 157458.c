rfbReleaseClientIterator(rfbClientIteratorPtr iterator)
{
  IF_PTHREADS(if(iterator->next) rfbDecrClientRef(iterator->next));
  free(iterator);
}