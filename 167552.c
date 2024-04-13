convert_page_layout (Catalog::PageLayout pageLayout)
{
  switch (pageLayout)
    {
    case Catalog::pageLayoutSinglePage:
      return POPPLER_PAGE_LAYOUT_SINGLE_PAGE;
    case Catalog::pageLayoutOneColumn:
      return POPPLER_PAGE_LAYOUT_ONE_COLUMN;
    case Catalog::pageLayoutTwoColumnLeft:
      return POPPLER_PAGE_LAYOUT_TWO_COLUMN_LEFT;
    case Catalog::pageLayoutTwoColumnRight:
      return POPPLER_PAGE_LAYOUT_TWO_COLUMN_RIGHT;
    case Catalog::pageLayoutTwoPageLeft:
      return POPPLER_PAGE_LAYOUT_TWO_PAGE_LEFT;
    case Catalog::pageLayoutTwoPageRight:
      return POPPLER_PAGE_LAYOUT_TWO_PAGE_RIGHT;
    case Catalog::pageLayoutNone:
    default:
      return POPPLER_PAGE_LAYOUT_UNSET;
    }
}