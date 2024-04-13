static void initAppData(AppData* data) {
	data->shareDesktop=TRUE;
	data->viewOnly=FALSE;
#ifdef LIBVNCSERVER_CONFIG_LIBVA
	data->encodingsString="h264 tight zrle ultra copyrect hextile zlib corre rre raw";
#else
	data->encodingsString="tight zrle ultra copyrect hextile zlib corre rre raw";
#endif
	data->useBGR233=FALSE;
	data->nColours=0;
	data->forceOwnCmap=FALSE;
	data->forceTrueColour=FALSE;
	data->requestedDepth=0;
	data->compressLevel=3;
	data->qualityLevel=5;
#ifdef LIBVNCSERVER_HAVE_LIBJPEG
	data->enableJPEG=TRUE;
#else
	data->enableJPEG=FALSE;
#endif
	data->useRemoteCursor=FALSE;
}