rfbSendFramebufferUpdate(rfbClientPtr cl,
                         sraRegionPtr givenUpdateRegion)
{
    sraRectangleIterator* i=NULL;
    sraRect rect;
    int nUpdateRegionRects;
    rfbFramebufferUpdateMsg *fu = (rfbFramebufferUpdateMsg *)cl->updateBuf;
    sraRegionPtr updateRegion,updateCopyRegion,tmpRegion;
    int dx, dy;
    rfbBool sendCursorShape = FALSE;
    rfbBool sendCursorPos = FALSE;
    rfbBool sendKeyboardLedState = FALSE;
    rfbBool sendSupportedMessages = FALSE;
    rfbBool sendSupportedEncodings = FALSE;
    rfbBool sendServerIdentity = FALSE;
    rfbBool result = TRUE;
    

    if(cl->screen->displayHook)
      cl->screen->displayHook(cl);

    /*
     * If framebuffer size was changed and the client supports NewFBSize
     * encoding, just send NewFBSize marker and return.
     */

    if (cl->useNewFBSize && cl->newFBSizePending) {
      LOCK(cl->updateMutex);
      cl->newFBSizePending = FALSE;
      UNLOCK(cl->updateMutex);
      fu->type = rfbFramebufferUpdate;
      fu->nRects = Swap16IfLE(1);
      cl->ublen = sz_rfbFramebufferUpdateMsg;
      if (!rfbSendNewFBSize(cl, cl->scaledScreen->width, cl->scaledScreen->height)) {
	if(cl->screen->displayFinishedHook)
	  cl->screen->displayFinishedHook(cl, FALSE);
        return FALSE;
      }
      result = rfbSendUpdateBuf(cl);
      if(cl->screen->displayFinishedHook)
	cl->screen->displayFinishedHook(cl, result);
      return result;
    }
    
    /*
     * If this client understands cursor shape updates, cursor should be
     * removed from the framebuffer. Otherwise, make sure it's put up.
     */

    if (cl->enableCursorShapeUpdates) {
      if (cl->cursorWasChanged && cl->readyForSetColourMapEntries)
	  sendCursorShape = TRUE;
    }

    /*
     * Do we plan to send cursor position update?
     */

    if (cl->enableCursorPosUpdates && cl->cursorWasMoved)
      sendCursorPos = TRUE;

    /*
     * Do we plan to send a keyboard state update?
     */
    if ((cl->enableKeyboardLedState) &&
	(cl->screen->getKeyboardLedStateHook!=NULL))
    {
        int x;
        x=cl->screen->getKeyboardLedStateHook(cl->screen);
        if (x!=cl->lastKeyboardLedState)
        {
            sendKeyboardLedState = TRUE;
            cl->lastKeyboardLedState=x;
        }
    }

    /*
     * Do we plan to send a rfbEncodingSupportedMessages?
     */
    if (cl->enableSupportedMessages)
    {
        sendSupportedMessages = TRUE;
        /* We only send this message ONCE <per setEncodings message received>
         * (We disable it here)
         */
        cl->enableSupportedMessages = FALSE;
    }
    /*
     * Do we plan to send a rfbEncodingSupportedEncodings?
     */
    if (cl->enableSupportedEncodings)
    {
        sendSupportedEncodings = TRUE;
        /* We only send this message ONCE <per setEncodings message received>
         * (We disable it here)
         */
        cl->enableSupportedEncodings = FALSE;
    }
    /*
     * Do we plan to send a rfbEncodingServerIdentity?
     */
    if (cl->enableServerIdentity)
    {
        sendServerIdentity = TRUE;
        /* We only send this message ONCE <per setEncodings message received>
         * (We disable it here)
         */
        cl->enableServerIdentity = FALSE;
    }

    LOCK(cl->updateMutex);

    /*
     * The modifiedRegion may overlap the destination copyRegion.  We remove
     * any overlapping bits from the copyRegion (since they'd only be
     * overwritten anyway).
     */
    
    sraRgnSubtract(cl->copyRegion,cl->modifiedRegion);

    /*
     * The client is interested in the region requestedRegion.  The region
     * which should be updated now is the intersection of requestedRegion
     * and the union of modifiedRegion and copyRegion.  If it's empty then
     * no update is needed.
     */

    updateRegion = sraRgnCreateRgn(givenUpdateRegion);
    if(cl->screen->progressiveSliceHeight>0) {
	    int height=cl->screen->progressiveSliceHeight,
	    	y=cl->progressiveSliceY;
	    sraRegionPtr bbox=sraRgnBBox(updateRegion);
	    sraRect rect;
	    if(sraRgnPopRect(bbox,&rect,0)) {
		sraRegionPtr slice;
		if(y<rect.y1 || y>=rect.y2)
		    y=rect.y1;
	    	slice=sraRgnCreateRect(0,y,cl->screen->width,y+height);
		sraRgnAnd(updateRegion,slice);
		sraRgnDestroy(slice);
	    }
	    sraRgnDestroy(bbox);
	    y+=height;
	    if(y>=cl->screen->height)
		    y=0;
	    cl->progressiveSliceY=y;
    }

    sraRgnOr(updateRegion,cl->copyRegion);
    if(!sraRgnAnd(updateRegion,cl->requestedRegion) &&
       sraRgnEmpty(updateRegion) &&
       (cl->enableCursorShapeUpdates ||
	(cl->cursorX == cl->screen->cursorX && cl->cursorY == cl->screen->cursorY)) &&
       !sendCursorShape && !sendCursorPos && !sendKeyboardLedState &&
       !sendSupportedMessages && !sendSupportedEncodings && !sendServerIdentity) {
      sraRgnDestroy(updateRegion);
      UNLOCK(cl->updateMutex);
      if(cl->screen->displayFinishedHook)
	cl->screen->displayFinishedHook(cl, TRUE);
      return TRUE;
    }

    /*
     * We assume that the client doesn't have any pixel data outside the
     * requestedRegion.  In other words, both the source and destination of a
     * copy must lie within requestedRegion.  So the region we can send as a
     * copy is the intersection of the copyRegion with both the requestedRegion
     * and the requestedRegion translated by the amount of the copy.  We set
     * updateCopyRegion to this.
     */

    updateCopyRegion = sraRgnCreateRgn(cl->copyRegion);
    sraRgnAnd(updateCopyRegion,cl->requestedRegion);
    tmpRegion = sraRgnCreateRgn(cl->requestedRegion);
    sraRgnOffset(tmpRegion,cl->copyDX,cl->copyDY);
    sraRgnAnd(updateCopyRegion,tmpRegion);
    sraRgnDestroy(tmpRegion);
    dx = cl->copyDX;
    dy = cl->copyDY;

    /*
     * Next we remove updateCopyRegion from updateRegion so that updateRegion
     * is the part of this update which is sent as ordinary pixel data (i.e not
     * a copy).
     */

    sraRgnSubtract(updateRegion,updateCopyRegion);

    /*
     * Finally we leave modifiedRegion to be the remainder (if any) of parts of
     * the screen which are modified but outside the requestedRegion.  We also
     * empty both the requestedRegion and the copyRegion - note that we never
     * carry over a copyRegion for a future update.
     */

     sraRgnOr(cl->modifiedRegion,cl->copyRegion);
     sraRgnSubtract(cl->modifiedRegion,updateRegion);
     sraRgnSubtract(cl->modifiedRegion,updateCopyRegion);

     sraRgnMakeEmpty(cl->requestedRegion);
     sraRgnMakeEmpty(cl->copyRegion);
     cl->copyDX = 0;
     cl->copyDY = 0;
   
     UNLOCK(cl->updateMutex);
   
    if (!cl->enableCursorShapeUpdates) {
      if(cl->cursorX != cl->screen->cursorX || cl->cursorY != cl->screen->cursorY) {
	rfbRedrawAfterHideCursor(cl,updateRegion);
	LOCK(cl->screen->cursorMutex);
	cl->cursorX = cl->screen->cursorX;
	cl->cursorY = cl->screen->cursorY;
	UNLOCK(cl->screen->cursorMutex);
	rfbRedrawAfterHideCursor(cl,updateRegion);
      }
      rfbShowCursor(cl);
    }

    /*
     * Now send the update.
     */
    
    rfbStatRecordMessageSent(cl, rfbFramebufferUpdate, 0, 0);
    if (cl->preferredEncoding == rfbEncodingCoRRE) {
        nUpdateRegionRects = 0;

        for(i = sraRgnGetIterator(updateRegion); sraRgnIteratorNext(i,&rect);){
            int x = rect.x1;
            int y = rect.y1;
            int w = rect.x2 - x;
            int h = rect.y2 - y;
	    int rectsPerRow, rows;
            /* We need to count the number of rects in the scaled screen */
            if (cl->screen!=cl->scaledScreen)
                rfbScaledCorrection(cl->screen, cl->scaledScreen, &x, &y, &w, &h, "rfbSendFramebufferUpdate");
	    rectsPerRow = (w-1)/cl->correMaxWidth+1;
	    rows = (h-1)/cl->correMaxHeight+1;
	    nUpdateRegionRects += rectsPerRow*rows;
        }
	sraRgnReleaseIterator(i); i=NULL;
    } else if (cl->preferredEncoding == rfbEncodingUltra) {
        nUpdateRegionRects = 0;
        
        for(i = sraRgnGetIterator(updateRegion); sraRgnIteratorNext(i,&rect);){
            int x = rect.x1;
            int y = rect.y1;
            int w = rect.x2 - x;
            int h = rect.y2 - y;
            /* We need to count the number of rects in the scaled screen */
            if (cl->screen!=cl->scaledScreen)
                rfbScaledCorrection(cl->screen, cl->scaledScreen, &x, &y, &w, &h, "rfbSendFramebufferUpdate");
            nUpdateRegionRects += (((h-1) / (ULTRA_MAX_SIZE( w ) / w)) + 1);
          }
        sraRgnReleaseIterator(i); i=NULL;
#ifdef LIBVNCSERVER_HAVE_LIBZ
    } else if (cl->preferredEncoding == rfbEncodingZlib) {
	nUpdateRegionRects = 0;

        for(i = sraRgnGetIterator(updateRegion); sraRgnIteratorNext(i,&rect);){
            int x = rect.x1;
            int y = rect.y1;
            int w = rect.x2 - x;
            int h = rect.y2 - y;
            /* We need to count the number of rects in the scaled screen */
            if (cl->screen!=cl->scaledScreen)
                rfbScaledCorrection(cl->screen, cl->scaledScreen, &x, &y, &w, &h, "rfbSendFramebufferUpdate");
	    nUpdateRegionRects += (((h-1) / (ZLIB_MAX_SIZE( w ) / w)) + 1);
	}
	sraRgnReleaseIterator(i); i=NULL;
#ifdef LIBVNCSERVER_HAVE_LIBJPEG
    } else if (cl->preferredEncoding == rfbEncodingTight) {
	nUpdateRegionRects = 0;

        for(i = sraRgnGetIterator(updateRegion); sraRgnIteratorNext(i,&rect);){
            int x = rect.x1;
            int y = rect.y1;
            int w = rect.x2 - x;
            int h = rect.y2 - y;
            int n;
            /* We need to count the number of rects in the scaled screen */
            if (cl->screen!=cl->scaledScreen)
                rfbScaledCorrection(cl->screen, cl->scaledScreen, &x, &y, &w, &h, "rfbSendFramebufferUpdate");
	    n = rfbNumCodedRectsTight(cl, x, y, w, h);
	    if (n == 0) {
		nUpdateRegionRects = 0xFFFF;
		break;
	    }
	    nUpdateRegionRects += n;
	}
	sraRgnReleaseIterator(i); i=NULL;
#endif
#endif
#if defined(LIBVNCSERVER_HAVE_LIBJPEG) && defined(LIBVNCSERVER_HAVE_LIBPNG)
    } else if (cl->preferredEncoding == rfbEncodingTightPng) {
	nUpdateRegionRects = 0;

        for(i = sraRgnGetIterator(updateRegion); sraRgnIteratorNext(i,&rect);){
            int x = rect.x1;
            int y = rect.y1;
            int w = rect.x2 - x;
            int h = rect.y2 - y;
            int n;
            /* We need to count the number of rects in the scaled screen */
            if (cl->screen!=cl->scaledScreen)
                rfbScaledCorrection(cl->screen, cl->scaledScreen, &x, &y, &w, &h, "rfbSendFramebufferUpdate");
	    n = rfbNumCodedRectsTight(cl, x, y, w, h);
	    if (n == 0) {
		nUpdateRegionRects = 0xFFFF;
		break;
	    }
	    nUpdateRegionRects += n;
	}
	sraRgnReleaseIterator(i); i=NULL;
#endif
    } else {
        nUpdateRegionRects = sraRgnCountRects(updateRegion);
    }

    fu->type = rfbFramebufferUpdate;
    if (nUpdateRegionRects != 0xFFFF) {
	if(cl->screen->maxRectsPerUpdate>0
	   /* CoRRE splits the screen into smaller squares */
	   && cl->preferredEncoding != rfbEncodingCoRRE
	   /* Ultra encoding splits rectangles up into smaller chunks */
           && cl->preferredEncoding != rfbEncodingUltra
#ifdef LIBVNCSERVER_HAVE_LIBZ
	   /* Zlib encoding splits rectangles up into smaller chunks */
	   && cl->preferredEncoding != rfbEncodingZlib
#ifdef LIBVNCSERVER_HAVE_LIBJPEG
	   /* Tight encoding counts the rectangles differently */
	   && cl->preferredEncoding != rfbEncodingTight
#endif
#endif
#ifdef LIBVNCSERVER_HAVE_LIBPNG
	   /* Tight encoding counts the rectangles differently */
	   && cl->preferredEncoding != rfbEncodingTightPng
#endif
	   && nUpdateRegionRects>cl->screen->maxRectsPerUpdate) {
	    sraRegion* newUpdateRegion = sraRgnBBox(updateRegion);
	    sraRgnDestroy(updateRegion);
	    updateRegion = newUpdateRegion;
	    nUpdateRegionRects = sraRgnCountRects(updateRegion);
	}
	fu->nRects = Swap16IfLE((uint16_t)(sraRgnCountRects(updateCopyRegion) +
					   nUpdateRegionRects +
					   !!sendCursorShape + !!sendCursorPos + !!sendKeyboardLedState +
					   !!sendSupportedMessages + !!sendSupportedEncodings + !!sendServerIdentity));
    } else {
	fu->nRects = 0xFFFF;
    }
    cl->ublen = sz_rfbFramebufferUpdateMsg;

   if (sendCursorShape) {
	cl->cursorWasChanged = FALSE;
	if (!rfbSendCursorShape(cl))
	    goto updateFailed;
    }
   
   if (sendCursorPos) {
	cl->cursorWasMoved = FALSE;
	if (!rfbSendCursorPos(cl))
	        goto updateFailed;
   }
   
   if (sendKeyboardLedState) {
       if (!rfbSendKeyboardLedState(cl))
           goto updateFailed;
   }

   if (sendSupportedMessages) {
       if (!rfbSendSupportedMessages(cl))
           goto updateFailed;
   }
   if (sendSupportedEncodings) {
       if (!rfbSendSupportedEncodings(cl))
           goto updateFailed;
   }
   if (sendServerIdentity) {
       if (!rfbSendServerIdentity(cl))
           goto updateFailed;
   }

    if (!sraRgnEmpty(updateCopyRegion)) {
	if (!rfbSendCopyRegion(cl,updateCopyRegion,dx,dy))
	        goto updateFailed;
    }

    for(i = sraRgnGetIterator(updateRegion); sraRgnIteratorNext(i,&rect);){
        int x = rect.x1;
        int y = rect.y1;
        int w = rect.x2 - x;
        int h = rect.y2 - y;

        /* We need to count the number of rects in the scaled screen */
        if (cl->screen!=cl->scaledScreen)
            rfbScaledCorrection(cl->screen, cl->scaledScreen, &x, &y, &w, &h, "rfbSendFramebufferUpdate");

        switch (cl->preferredEncoding) {
	case -1:
        case rfbEncodingRaw:
            if (!rfbSendRectEncodingRaw(cl, x, y, w, h))
	        goto updateFailed;
            break;
        case rfbEncodingRRE:
            if (!rfbSendRectEncodingRRE(cl, x, y, w, h))
	        goto updateFailed;
            break;
        case rfbEncodingCoRRE:
            if (!rfbSendRectEncodingCoRRE(cl, x, y, w, h))
	        goto updateFailed;
	    break;
        case rfbEncodingHextile:
            if (!rfbSendRectEncodingHextile(cl, x, y, w, h))
	        goto updateFailed;
            break;
        case rfbEncodingUltra:
            if (!rfbSendRectEncodingUltra(cl, x, y, w, h))
                goto updateFailed;
            break;
#ifdef LIBVNCSERVER_HAVE_LIBZ
	case rfbEncodingZlib:
	    if (!rfbSendRectEncodingZlib(cl, x, y, w, h))
	        goto updateFailed;
	    break;
       case rfbEncodingZRLE:
       case rfbEncodingZYWRLE:
           if (!rfbSendRectEncodingZRLE(cl, x, y, w, h))
	       goto updateFailed;
           break;
#endif
#if defined(LIBVNCSERVER_HAVE_LIBJPEG) && (defined(LIBVNCSERVER_HAVE_LIBZ) || defined(LIBVNCSERVER_HAVE_LIBPNG))
	case rfbEncodingTight:
	    if (!rfbSendRectEncodingTight(cl, x, y, w, h))
	        goto updateFailed;
	    break;
#ifdef LIBVNCSERVER_HAVE_LIBPNG
	case rfbEncodingTightPng:
	    if (!rfbSendRectEncodingTightPng(cl, x, y, w, h))
	        goto updateFailed;
	    break;
#endif
#endif
        }
    }
    if (i) {
        sraRgnReleaseIterator(i);
        i = NULL;
    }

    if ( nUpdateRegionRects == 0xFFFF &&
	 !rfbSendLastRectMarker(cl) )
	    goto updateFailed;

    if (!rfbSendUpdateBuf(cl)) {
updateFailed:
	result = FALSE;
    }

    if (!cl->enableCursorShapeUpdates) {
      rfbHideCursor(cl);
    }

    if(i)
        sraRgnReleaseIterator(i);
    sraRgnDestroy(updateRegion);
    sraRgnDestroy(updateCopyRegion);

    if(cl->screen->displayFinishedHook)
      cl->screen->displayFinishedHook(cl, result);
    return result;
}