	virtual bool OnLoad(const CString& sArgStr, CString& sMessage) {
		if (sArgStr.empty() || CModInfo::GlobalModule != GetType())
			return true;

		// We don't accept any arguments, but for backwards
		// compatibility we have to do some magic here.
		sMessage = "Arguments converted to new syntax";

		bool bSSL = false;
		bool bIPv6 = false;
		bool bShareIRCPorts = true;
		unsigned short uPort = 8080;
		CString sArgs(sArgStr);
		CString sPort;
		CString sListenHost;
		CString sURIPrefix;

		while (sArgs.Left(1) == "-") {
			CString sOpt = sArgs.Token(0);
			sArgs = sArgs.Token(1, true);

			if (sOpt.Equals("-IPV6")) {
				bIPv6 = true;
			} else if (sOpt.Equals("-IPV4")) {
				bIPv6 = false;
			} else if (sOpt.Equals("-noircport")) {
				bShareIRCPorts = false;
			} else {
				// Uhm... Unknown option? Let's just ignore all
				// arguments, older versions would have returned
				// an error and denied loading
				return true;
			}
		}

		// No arguments left: Only port sharing
		if (sArgs.empty() && bShareIRCPorts)
			return true;

		if (sArgs.find(" ") != CString::npos) {
			sListenHost = sArgs.Token(0);
			sPort = sArgs.Token(1, true);
		} else {
			sPort = sArgs;
		}

		if (sPort.Left(1) == "+") {
			sPort.TrimLeft("+");
			bSSL = true;
		}

		if (!sPort.empty()) {
			uPort = sPort.ToUShort();
		}

		if (!bShareIRCPorts) {
			// Make all existing listeners IRC-only
			const vector<CListener*>& vListeners = CZNC::Get().GetListeners();
			vector<CListener*>::const_iterator it;
			for (it = vListeners.begin(); it != vListeners.end(); ++it) {
				(*it)->SetAcceptType(CListener::ACCEPT_IRC);
			}
		}

		// Now turn that into a listener instance
		CListener *pListener = new CListener(uPort, sListenHost, sURIPrefix, bSSL,
				(!bIPv6 ? ADDR_IPV4ONLY : ADDR_ALL), CListener::ACCEPT_HTTP);

		if (!pListener->Listen()) {
			sMessage = "Failed to add backwards-compatible listener";
			return false;
		}
		CZNC::Get().AddListener(pListener);

		SetArgs("");
		return true;
	}