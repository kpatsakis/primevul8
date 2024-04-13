rfbClientIteratorHead(rfbClientIteratorPtr i)
{
#ifdef LIBVNCSERVER_HAVE_LIBPTHREAD
  if(i->next != 0) {
    rfbDecrClientRef(i->next);
    rfbIncrClientRef(i->screen->clientHead);
  }
#endif
  LOCK(rfbClientListMutex);
  i->next = i->screen->clientHead;
  UNLOCK(rfbClientListMutex);
  return i->next;
}