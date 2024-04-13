cmsBool IsDegenerated(const cmsToneCurve* g)
{
    int i, Zeros = 0, Poles = 0;
    int nEntries = g ->nEntries;

    for (i=0; i < nEntries; i++) {

        if (g ->Table16[i] == 0x0000) Zeros++;
        if (g ->Table16[i] == 0xffff) Poles++;
    }

    if (Zeros == 1 && Poles == 1) return FALSE;  // For linear tables
    if (Zeros > (nEntries / 20)) return TRUE;  // Degenerated, many zeros
    if (Poles > (nEntries / 20)) return TRUE;  // Degenerated, many poles

    return FALSE;
}