int LibRaw::find_ifd_by_offset(int o)
{
    for(int i = 0; i < libraw_internal_data.identify_data.tiff_nifds && i < LIBRAW_IFD_MAXCOUNT; i++)
        if(tiff_ifd[i].offset == o)
            return i;
    return -1;
}