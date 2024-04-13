static dng_ifd* search_for_ifd(const dng_info& info, uint64 offset, ushort w, ushort h)
{
    dng_ifd *ret = 0;
    ret = search_single_ifd(info.fIFD,offset);
    if(!ret) ret = search_single_ifd(info.fChainedIFD,offset);
    if(!ret)
        for(int c = 0; !ret && c < info.fChainedSubIFD.size(); c++)
            ret = search_single_ifd(info.fChainedSubIFD[c],offset);
    if(ret && (ret->fImageLength == h) && ret->fImageWidth == w)
        return ret;
    return 0;
}