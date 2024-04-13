cmsTagTypeSignature DecideTextDescType(cmsFloat64Number ICCVersion, const void *Data)
{
    if (ICCVersion >= 4.0)
        return cmsSigMultiLocalizedUnicodeType;

    return cmsSigTextDescriptionType;

    cmsUNUSED_PARAMETER(Data);
}