cmsBool WriteOffsetArray(cmsIOHANDLER* io,  _cmsDICarray* a, cmsUInt32Number Count, cmsUInt32Number Length)
{
    cmsUInt32Number i;

    for (i=0; i < Count; i++) {

        if (!WriteOneElem(io, &a -> Name, i)) return FALSE;
        if (!WriteOneElem(io, &a -> Value, i))  return FALSE;

        if (Length > 16) {

            if (!WriteOneElem(io, &a -> DisplayName, i))  return FALSE;
        }

        if (Length > 24) {

            if (!WriteOneElem(io, &a -> DisplayValue, i))  return FALSE;
        }
    }

    return TRUE;
}