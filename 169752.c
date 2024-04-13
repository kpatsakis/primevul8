ImagingResample(Imaging imIn, int xsize, int ysize, int filter)
{
    Imaging imTemp1, imTemp2, imTemp3;
    Imaging imOut;

    if (strcmp(imIn->mode, "P") == 0 || strcmp(imIn->mode, "1") == 0)
        return (Imaging) ImagingError_ModeError();

    if (imIn->type == IMAGING_TYPE_SPECIAL)
        return (Imaging) ImagingError_ModeError();

    /* two-pass resize, first pass */
    imTemp1 = ImagingResampleHorizontal(imIn, xsize, filter);
    if ( ! imTemp1)
        return NULL;

    /* transpose image once */
    imTemp2 = ImagingTransposeToNew(imTemp1);
    ImagingDelete(imTemp1);
    if ( ! imTemp2)
        return NULL;

    /* second pass */
    imTemp3 = ImagingResampleHorizontal(imTemp2, ysize, filter);
    ImagingDelete(imTemp2);
    if ( ! imTemp3)
        return NULL;

    /* transpose result */
    imOut = ImagingTransposeToNew(imTemp3);
    ImagingDelete(imTemp3);
    if ( ! imOut)
        return NULL;

    return imOut;
}