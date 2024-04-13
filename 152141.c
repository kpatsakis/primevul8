MagickPrivate void GetPixelCacheMethods(CacheMethods *cache_methods)
{
  assert(cache_methods != (CacheMethods *) NULL);
  (void) ResetMagickMemory(cache_methods,0,sizeof(*cache_methods));
  cache_methods->get_virtual_pixel_handler=GetVirtualPixelCache;
  cache_methods->get_virtual_pixels_handler=GetVirtualPixelsCache;
  cache_methods->get_virtual_metacontent_from_handler=
    GetVirtualMetacontentFromCache;
  cache_methods->get_one_virtual_pixel_from_handler=GetOneVirtualPixelFromCache;
  cache_methods->get_authentic_pixels_handler=GetAuthenticPixelsCache;
  cache_methods->get_authentic_metacontent_from_handler=
    GetAuthenticMetacontentFromCache;
  cache_methods->get_authentic_pixels_from_handler=GetAuthenticPixelsFromCache;
  cache_methods->get_one_authentic_pixel_from_handler=
    GetOneAuthenticPixelFromCache;
  cache_methods->queue_authentic_pixels_handler=QueueAuthenticPixelsCache;
  cache_methods->sync_authentic_pixels_handler=SyncAuthenticPixelsCache;
  cache_methods->destroy_pixel_handler=DestroyImagePixelCache;
}