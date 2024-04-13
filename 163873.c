void DoEmbedProfile(const char* ProfileFile)
{
    FILE* f;
    size_t size, EmbedLen;
    cmsUInt8Number* EmbedBuffer;

        f = fopen(ProfileFile, "rb");
        if (f == NULL) return;

        size = cmsfilelength(f);
        EmbedBuffer = (cmsUInt8Number*) malloc(size + 1);
        EmbedLen = fread(EmbedBuffer, 1, size, f);
        fclose(f);
        EmbedBuffer[EmbedLen] = 0;

        write_icc_profile (&Compressor, EmbedBuffer, EmbedLen);
        free(EmbedBuffer);
}