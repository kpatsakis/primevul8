    BasicIo::AutoPtr ImageFactory::createIo(const std::wstring& wpath, bool useCurl)
    {
        Protocol fProt = fileProtocol(wpath);
#if EXV_USE_SSH == 1
        if (fProt == pSsh || fProt == pSftp) {
            return BasicIo::AutoPtr(new SshIo(wpath));
        }
#endif
#if EXV_USE_CURL == 1
        if (useCurl && (fProt == pHttp || fProt == pHttps || fProt == pFtp)) {
            return BasicIo::AutoPtr(new CurlIo(wpath));
        }
#endif
        if (fProt == pHttp)
            return BasicIo::AutoPtr(new HttpIo(wpath));
        if (fProt == pFileUri)
            return BasicIo::AutoPtr(new FileIo(pathOfFileUrl(wpath)));
        if (fProt == pStdin || fProt == pDataUri)
            return BasicIo::AutoPtr(new XPathIo(wpath)); // may throw
        return BasicIo::AutoPtr(new FileIo(wpath));
    } // ImageFactory::createIo