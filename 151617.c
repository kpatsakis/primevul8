cmsTagTypeSignature DecideTextType(cmsFloat64Number ICCVersion, const void *Data)
{
    if (ICCVersion >= 4.0)
        return cmsSigMultiLocalizedUnicodeType;

    return cmsSigTextType;

    cmsUNUSED_PARAMETER(Data);
}