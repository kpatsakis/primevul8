rfbClientIteratorNext(rfbClientIteratorPtr i)
{
  if(i->next == 0) {
    LOCK(rfbClientListMutex);
    i->next = i->screen->clientHead;
    UNLOCK(rfbClientListMutex);
  } else {
    IF_PTHREADS(rfbClientPtr cl = i->next);
    i->next = i->next->next;
    IF_PTHREADS(rfbDecrClientRef(cl));
  }

#ifdef LIBVNCSERVER_HAVE_LIBPTHREAD
    if(!i->closedToo)
      while(i->next && i->next->sock<0)
        i->next = i->next->next;
    if(i->next)
      rfbIncrClientRef(i->next);
#endif

    return i->next;
}