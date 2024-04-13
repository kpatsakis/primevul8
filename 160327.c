static dng_ifd* search_single_ifd(const std::vector <dng_ifd *>& v, uint64 offset)
{
    for(int i = 0; i < v.size(); i++)
    {
        if(!v[i]) continue;;
        if(v[i]->fTileOffsetsOffset == offset) return v[i];
        if(v[i]->fTileOffsetsCount == 1 && v[i]->fTileOffset[0] == offset) return v[i];
    }
    return NULL;
}