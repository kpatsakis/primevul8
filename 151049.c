Controller::determineHeaderSizeForSessionProtocol(Request *req,
	SessionProtocolWorkingState &state, string delta_monotonic)
{
	unsigned int dataSize = sizeof(boost::uint32_t);

	state.path        = req->getPathWithoutQueryString();
	state.hasBaseURI  = req->options.baseURI != P_STATIC_STRING("/")
		&& startsWith(state.path, req->options.baseURI);
	if (state.hasBaseURI) {
		state.path = state.path.substr(req->options.baseURI.size());
		if (state.path.empty()) {
			state.path = P_STATIC_STRING("/");
		}
	}
	state.queryString = req->getQueryString();
	state.methodStr   = StaticString(http_method_str(req->method));
	state.remoteAddr  = req->secureHeaders.lookup(REMOTE_ADDR);
	state.remotePort  = req->secureHeaders.lookup(REMOTE_PORT);
	state.remoteUser  = req->secureHeaders.lookup(REMOTE_USER);
	state.contentType   = req->headers.lookup(HTTP_CONTENT_TYPE);
	if (req->hasBody()) {
		state.contentLength = req->headers.lookup(HTTP_CONTENT_LENGTH);
	} else {
		state.contentLength = NULL;
	}
	if (req->envvars != NULL) {
		size_t len = modp_b64_decode_len(req->envvars->size);
		state.environmentVariablesData = (char *) malloc(len);
		if (state.environmentVariablesData == NULL) {
			throw RuntimeException("Unable to allocate memory for base64 "
				"decoding of environment variables");
		}
		len = modp_b64_decode(state.environmentVariablesData,
			req->envvars->start->data,
			req->envvars->size);
		if (len == (size_t) -1) {
			throw RuntimeException("Unable to base64 decode environment variables");
		}
		state.environmentVariablesSize = len;
	}

	dataSize += sizeof("REQUEST_URI");
	dataSize += req->path.size + 1;

	dataSize += sizeof("PATH_INFO");
	dataSize += state.path.size() + 1;

	dataSize += sizeof("SCRIPT_NAME");
	if (state.hasBaseURI) {
		dataSize += req->options.baseURI.size();
	} else {
		dataSize += sizeof("");
	}

	dataSize += sizeof("QUERY_STRING");
	dataSize += state.queryString.size() + 1;

	dataSize += sizeof("REQUEST_METHOD");
	dataSize += state.methodStr.size() + 1;

	if (req->host != NULL && req->host->size > 0) {
		const LString *host = psg_lstr_make_contiguous(req->host, req->pool);
		const char *sep = (const char *) memchr(host->start->data, ':', host->size);
		if (sep != NULL) {
			state.serverName = StaticString(host->start->data, sep - host->start->data);
			state.serverPort = StaticString(sep + 1,
				host->start->data + host->size - sep - 1);
		} else {
			state.serverName = StaticString(host->start->data, host->size);
			if (req->https) {
				state.serverPort = P_STATIC_STRING("443");
			} else {
				state.serverPort = P_STATIC_STRING("80");
			}
		}
	} else {
		state.serverName = defaultServerName;
		state.serverPort = defaultServerPort;
	}

	dataSize += sizeof("SERVER_NAME");
	dataSize += state.serverName.size() + 1;

	dataSize += sizeof("SERVER_PORT");
	dataSize += state.serverPort.size() + 1;

	dataSize += sizeof("SERVER_SOFTWARE");
	dataSize += serverSoftware.size() + 1;

	dataSize += sizeof("SERVER_PROTOCOL");
	dataSize += sizeof("HTTP/1.1");

	dataSize += sizeof("REMOTE_ADDR");
	if (state.remoteAddr != NULL) {
		dataSize += state.remoteAddr->size + 1;
	} else {
		dataSize += sizeof("127.0.0.1");
	}

	dataSize += sizeof("REMOTE_PORT");
	if (state.remotePort != NULL) {
		dataSize += state.remotePort->size + 1;
	} else {
		dataSize += sizeof("0");
	}

	if (state.remoteUser != NULL) {
		dataSize += sizeof("REMOTE_USER");
		dataSize += state.remoteUser->size + 1;
	}

	if (state.contentType != NULL) {
		dataSize += sizeof("CONTENT_TYPE");
		dataSize += state.contentType->size + 1;
	}

	if (state.contentLength != NULL) {
		dataSize += sizeof("CONTENT_LENGTH");
		dataSize += state.contentLength->size + 1;
	}

	dataSize += sizeof("PASSENGER_CONNECT_PASSWORD");
	dataSize += ApplicationPool2::ApiKey::SIZE + 1;

	if (req->https) {
		dataSize += sizeof("HTTPS");
		dataSize += sizeof("on");
	}

	if (req->options.analytics) {
		dataSize += sizeof("PASSENGER_TXN_ID");
		dataSize += req->options.transaction->getTxnId().size() + 1;

		dataSize += sizeof("PASSENGER_DELTA_MONOTONIC");
		dataSize += delta_monotonic.size() + 1;
	}

	if (req->upgraded()) {
		dataSize += sizeof("HTTP_CONNECTION");
		dataSize += sizeof("upgrade");
	}

	ServerKit::HeaderTable::Iterator it(req->headers);
	while (*it != NULL) {
		dataSize += sizeof("HTTP_") - 1 + it->header->key.size + 1;
		dataSize += it->header->val.size + 1;
		it.next();
	}

	if (state.environmentVariablesData != NULL) {
		dataSize += state.environmentVariablesSize;
	}

	return dataSize + 1;
}