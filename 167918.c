static int seed_from_windows_cryptoapi(uint32_t *seed)
{
    HINSTANCE hAdvAPI32 = NULL;
    CRYPTACQUIRECONTEXTA pCryptAcquireContext = NULL;
    CRYPTGENRANDOM pCryptGenRandom = NULL;
    CRYPTRELEASECONTEXT pCryptReleaseContext = NULL;
    HCRYPTPROV hCryptProv = 0;
    BYTE data[sizeof(uint32_t)];
    int ok;

    hAdvAPI32 = GetModuleHandle("advapi32.dll");
    if(hAdvAPI32 == NULL)
        return 1;

    pCryptAcquireContext = (CRYPTACQUIRECONTEXTA)GetProcAddress(hAdvAPI32, "CryptAcquireContextA");
    if (!pCryptAcquireContext)
        return 1;

    pCryptGenRandom = (CRYPTGENRANDOM)GetProcAddress(hAdvAPI32, "CryptGenRandom");
    if (!pCryptGenRandom)
        return 1;

    pCryptReleaseContext = (CRYPTRELEASECONTEXT)GetProcAddress(hAdvAPI32, "CryptReleaseContext");
    if (!pCryptReleaseContext)
        return 1;

    if (!pCryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
        return 1;

    ok = CryptGenRandom(hCryptProv, sizeof(uint32_t), data);
    pCryptReleaseContext(hCryptProv, 0);

    if (!ok)
        return 1;

    *seed = buf_to_uint32((char *)data);
    return 0;
}