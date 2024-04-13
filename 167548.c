convert_page_mode (Catalog::PageMode pageMode)
{
  switch (pageMode)
    {
    case Catalog::pageModeOutlines:
      return POPPLER_PAGE_MODE_USE_OUTLINES;
    case Catalog::pageModeThumbs:
      return POPPLER_PAGE_MODE_USE_THUMBS;
    case Catalog::pageModeFullScreen:
      return POPPLER_PAGE_MODE_FULL_SCREEN;
    case Catalog::pageModeOC:
      return POPPLER_PAGE_MODE_USE_OC;
    case Catalog::pageModeAttach:
      return POPPLER_PAGE_MODE_USE_ATTACHMENTS;
    case Catalog::pageModeNone:
    default:
      return POPPLER_PAGE_MODE_UNSET;
    }
}