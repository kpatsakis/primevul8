rfbSendSupportedEncodings(rfbClientPtr cl)
{
    rfbFramebufferUpdateRectHeader rect;
    static uint32_t supported[] = {
        rfbEncodingRaw,
	rfbEncodingCopyRect,
	rfbEncodingRRE,
	rfbEncodingCoRRE,
	rfbEncodingHextile,
#ifdef LIBVNCSERVER_HAVE_LIBZ
	rfbEncodingZlib,
	rfbEncodingZRLE,
	rfbEncodingZYWRLE,
#endif
#ifdef LIBVNCSERVER_HAVE_LIBJPEG
	rfbEncodingTight,
#endif
#ifdef LIBVNCSERVER_HAVE_LIBPNG
	rfbEncodingTightPng,
#endif
	rfbEncodingUltra,
	rfbEncodingUltraZip,
	rfbEncodingXCursor,
	rfbEncodingRichCursor,
	rfbEncodingPointerPos,
	rfbEncodingLastRect,
	rfbEncodingNewFBSize,
	rfbEncodingKeyboardLedState,
	rfbEncodingSupportedMessages,
	rfbEncodingSupportedEncodings,
	rfbEncodingServerIdentity,
    };
    uint32_t nEncodings = sizeof(supported) / sizeof(supported[0]), i;

    /* think rfbSetEncodingsMsg */

    if (cl->ublen + sz_rfbFramebufferUpdateRectHeader
                  + (nEncodings * sizeof(uint32_t)) > UPDATE_BUF_SIZE) {
        if (!rfbSendUpdateBuf(cl))
            return FALSE;
    }

    rect.encoding = Swap32IfLE(rfbEncodingSupportedEncodings);
    rect.r.x = 0;
    rect.r.y = 0;
    rect.r.w = Swap16IfLE(nEncodings * sizeof(uint32_t));
    rect.r.h = Swap16IfLE(nEncodings);

    memcpy(&cl->updateBuf[cl->ublen], (char *)&rect,
        sz_rfbFramebufferUpdateRectHeader);
    cl->ublen += sz_rfbFramebufferUpdateRectHeader;

    for (i = 0; i < nEncodings; i++) {
        uint32_t encoding = Swap32IfLE(supported[i]);
	memcpy(&cl->updateBuf[cl->ublen], (char *)&encoding, sizeof(encoding));
	cl->ublen += sizeof(encoding);
    }

    rfbStatRecordEncodingSent(cl, rfbEncodingSupportedEncodings,
        sz_rfbFramebufferUpdateRectHeader+(nEncodings * sizeof(uint32_t)),
        sz_rfbFramebufferUpdateRectHeader+(nEncodings * sizeof(uint32_t)));

    if (!rfbSendUpdateBuf(cl))
        return FALSE;

    return TRUE;
}