Controller::constructHeaderBuffersForHttpProtocol(Request *req, struct iovec *buffers,
	unsigned int maxbuffers, unsigned int & restrict_ref nbuffers,
	unsigned int & restrict_ref dataSize, HttpHeaderConstructionCache &cache)
{
	#define BEGIN_PUSH_NEXT_BUFFER() \
		do { \
			if (buffers != NULL && i >= maxbuffers) { \
				return false; \
			} \
		} while (false)
	#define INC_BUFFER_ITER(i) \
		do { \
			i++; \
		} while (false)
	#define PUSH_STATIC_BUFFER(buf) \
		do { \
			BEGIN_PUSH_NEXT_BUFFER(); \
			if (buffers != NULL) { \
				buffers[i].iov_base = (void *) buf; \
				buffers[i].iov_len  = sizeof(buf) - 1; \
			} \
			INC_BUFFER_ITER(i); \
			dataSize += sizeof(buf) - 1; \
		} while (false)

	ServerKit::HeaderTable::Iterator it(req->headers);
	const LString::Part *part;
	unsigned int i = 0;

	nbuffers = 0;
	dataSize = 0;

	if (!cache.cached) {
		cache.methodStr  = http_method_str(req->method);
		cache.remoteAddr = req->secureHeaders.lookup(REMOTE_ADDR);
		cache.setCookie  = req->headers.lookup(ServerKit::HTTP_SET_COOKIE);
		cache.cached     = true;
	}

	if (buffers != NULL) {
		BEGIN_PUSH_NEXT_BUFFER();
		buffers[i].iov_base = (void *) cache.methodStr.data();
		buffers[i].iov_len  = cache.methodStr.size();
	}
	INC_BUFFER_ITER(i);
	dataSize += cache.methodStr.size();

	PUSH_STATIC_BUFFER(" ");

	if (buffers != NULL) {
		BEGIN_PUSH_NEXT_BUFFER();
		buffers[i].iov_base = (void *) req->path.start->data;
		buffers[i].iov_len  = req->path.size;
	}
	INC_BUFFER_ITER(i);
	dataSize += req->path.size;

	if (req->upgraded()) {
		PUSH_STATIC_BUFFER(" HTTP/1.1\r\nConnection: upgrade\r\n");
	} else {
		PUSH_STATIC_BUFFER(" HTTP/1.1\r\nConnection: close\r\n");
	}

	if (cache.setCookie != NULL) {
		LString::Part *part;

		PUSH_STATIC_BUFFER("Set-Cookie: ");
		part = cache.setCookie->start;
		while (part != NULL) {
			if (part->size == 1 && part->data[0] == '\n') {
				// HeaderTable joins multiple Set-Cookie headers together using \n.
				PUSH_STATIC_BUFFER("\r\nSet-Cookie: ");
			} else {
				if (buffers != NULL) {
					BEGIN_PUSH_NEXT_BUFFER();
					buffers[i].iov_base = (void *) part->data;
					buffers[i].iov_len  = part->size;
				}
				INC_BUFFER_ITER(i);
				dataSize += part->size;
			}
			part = part->next;
		}
		PUSH_STATIC_BUFFER("\r\n");
	}

	while (*it != NULL) {
		if ((it->header->hash == HTTP_CONNECTION.hash()
		  || it->header->hash == ServerKit::HTTP_SET_COOKIE.hash())
		 && (psg_lstr_cmp(&it->header->key, P_STATIC_STRING("connection"))
		  || psg_lstr_cmp(&it->header->key, ServerKit::HTTP_SET_COOKIE)))
		{
			it.next();
			continue;
		}

		part = it->header->key.start;
		while (part != NULL) {
			if (buffers != NULL) {
				BEGIN_PUSH_NEXT_BUFFER();
				buffers[i].iov_base = (void *) part->data;
				buffers[i].iov_len  = part->size;
			}
			INC_BUFFER_ITER(i);
			part = part->next;
		}
		dataSize += it->header->key.size;

		PUSH_STATIC_BUFFER(": ");

		part = it->header->val.start;
		while (part != NULL) {
			if (buffers != NULL) {
				BEGIN_PUSH_NEXT_BUFFER();
				buffers[i].iov_base = (void *) part->data;
				buffers[i].iov_len  = part->size;
			}
			INC_BUFFER_ITER(i);
			part = part->next;
		}
		dataSize += it->header->val.size;

		PUSH_STATIC_BUFFER("\r\n");

		it.next();
	}

	if (req->https) {
		PUSH_STATIC_BUFFER("X-Forwarded-Proto: https\r\n");
		PUSH_STATIC_BUFFER("!~Passenger-Proto: https\r\n");
	}

	if (cache.remoteAddr != NULL && cache.remoteAddr->size > 0) {
		PUSH_STATIC_BUFFER("X-Forwarded-For: ");

		part = cache.remoteAddr->start;
		while (part != NULL) {
			if (buffers != NULL) {
				BEGIN_PUSH_NEXT_BUFFER();
				buffers[i].iov_base = (void *) part->data;
				buffers[i].iov_len  = part->size;
			}
			INC_BUFFER_ITER(i);
			part = part->next;
		}
		dataSize += cache.remoteAddr->size;

		PUSH_STATIC_BUFFER("\r\n");

		PUSH_STATIC_BUFFER("!~Passenger-Client-Address: ");

		part = cache.remoteAddr->start;
		while (part != NULL) {
			if (buffers != NULL) {
				BEGIN_PUSH_NEXT_BUFFER();
				buffers[i].iov_base = (void *) part->data;
				buffers[i].iov_len  = part->size;
			}
			INC_BUFFER_ITER(i);
			part = part->next;
		}
		dataSize += cache.remoteAddr->size;

		PUSH_STATIC_BUFFER("\r\n");
	}

	if (req->envvars != NULL) {
		PUSH_STATIC_BUFFER("!~Passenger-Envvars: ");
		if (buffers != NULL) {
			BEGIN_PUSH_NEXT_BUFFER();
			buffers[i].iov_base = (void *) req->envvars->start->data;
			buffers[i].iov_len = req->envvars->size;
		}
		INC_BUFFER_ITER(i);
		dataSize += req->envvars->size;
		PUSH_STATIC_BUFFER("\r\n");
	}

	if (req->options.analytics) {
		PUSH_STATIC_BUFFER("!~Passenger-Txn-Id: ");

		if (buffers != NULL) {
			BEGIN_PUSH_NEXT_BUFFER();
			buffers[i].iov_base = (void *) req->options.transaction->getTxnId().data();
			buffers[i].iov_len  = req->options.transaction->getTxnId().size();
		}
		INC_BUFFER_ITER(i);
		dataSize += req->options.transaction->getTxnId().size();

		PUSH_STATIC_BUFFER("\r\n");
	}

	PUSH_STATIC_BUFFER("\r\n");

	nbuffers = i;
	return true;

	#undef BEGIN_PUSH_NEXT_BUFFER
	#undef INC_BUFFER_ITER
	#undef PUSH_STATIC_BUFFER
}