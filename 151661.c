cmsBool WriteOneElem(cmsIOHANDLER* io,  _cmsDICelem* e, cmsUInt32Number i)
{
    if (!_cmsWriteUInt32Number(io, e->Offsets[i])) return FALSE;
    if (!_cmsWriteUInt32Number(io, e ->Sizes[i])) return FALSE;

    return TRUE;
}