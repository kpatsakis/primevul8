static rsRetVal uncompressMessage(smsg_t *pMsg)
{
	DEFiRet;
	uchar *deflateBuf = NULL;
	uLongf iLenDefBuf;
	uchar *pszMsg;
	size_t lenMsg;
	
	assert(pMsg != NULL);
	pszMsg = pMsg->pszRawMsg;
	lenMsg = pMsg->iLenRawMsg;

	/* we first need to check if we have a compressed record. If so,
	 * we must decompress it.
	 */
	if(lenMsg > 0 && *pszMsg == 'z') { /* compressed data present? (do NOT change order if conditions!) */
		/* we have compressed data, so let's deflate it. We support a maximum
		 * message size of iMaxLine. If it is larger, an error message is logged
		 * and the message is dropped. We do NOT try to decompress larger messages
		 * as such might be used for denial of service. It might happen to later
		 * builds that such functionality be added as an optional, operator-configurable
		 * feature.
		 */
		int ret;
		iLenDefBuf = glbl.GetMaxLine();
		CHKmalloc(deflateBuf = MALLOC(iLenDefBuf + 1));
		ret = uncompress((uchar *) deflateBuf, &iLenDefBuf, (uchar *) pszMsg+1, lenMsg-1);
		DBGPRINTF("Compressed message uncompressed with status %d, length: new %ld, old %d.\n",
		        ret, (long) iLenDefBuf, (int) (lenMsg-1));
		/* Now check if the uncompression worked. If not, there is not much we can do. In
		 * that case, we log an error message but ignore the message itself. Storing the
		 * compressed text is dangerous, as it contains control characters. So we do
		 * not do this. If someone would like to have a copy, this code here could be
		 * modified to do a hex-dump of the buffer in question. We do not include
		 * this functionality right now.
		 * rgerhards, 2006-12-07
		 */
		if(ret != Z_OK) {
			errmsg.LogError(0, NO_ERRCODE, "Uncompression of a message failed with return code %d "
			            "- enable debug logging if you need further information. "
				    "Message ignored.", ret);
			FINALIZE; /* unconditional exit, nothing left to do... */
		}
		MsgSetRawMsg(pMsg, (char*)deflateBuf, iLenDefBuf);
	}
finalize_it:
	if(deflateBuf != NULL)
		free(deflateBuf);

	RETiRet;
}