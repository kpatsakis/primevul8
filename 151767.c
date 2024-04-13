cmsTagTypeSignature DecideLUTtypeB2A(cmsFloat64Number ICCVersion, const void *Data)
{
    cmsPipeline* Lut = (cmsPipeline*) Data;

    if (ICCVersion < 4.0) {
        if (Lut ->SaveAs8Bits) return cmsSigLut8Type;
        return cmsSigLut16Type;
    }
    else {
         return cmsSigLutBtoAType;
    }
}