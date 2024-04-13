cmsBool  SaveOneChromaticity(cmsFloat64Number x, cmsFloat64Number y, cmsIOHANDLER* io)
{
    if (!_cmsWriteUInt32Number(io, _cmsDoubleTo15Fixed16(x))) return FALSE;
    if (!_cmsWriteUInt32Number(io, _cmsDoubleTo15Fixed16(y))) return FALSE;

    return TRUE;
}