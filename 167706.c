	virtual bool OnWebRequest(CWebSock& WebSock, const CString& sPageName, CTemplate& Tmpl) {
		CSmartPtr<CWebSession> spSession = WebSock.GetSession();

		if (sPageName == "settings") {
			// Admin Check
			if (!spSession->IsAdmin()) {
				return false;
			}

			return SettingsPage(WebSock, Tmpl);
		} else if (sPageName == "adduser") {
			// Admin Check
			if (!spSession->IsAdmin()) {
				return false;
			}

			return UserPage(WebSock, Tmpl);
		} else if (sPageName == "addnetwork") {
			CUser* pUser = SafeGetUserFromParam(WebSock);

			// Admin||Self Check
			if (!spSession->IsAdmin() && (!spSession->GetUser() || spSession->GetUser() != pUser)) {
				return false;
			}

			if (pUser) {
				return NetworkPage(WebSock, Tmpl, pUser);
			}

			WebSock.PrintErrorPage("No such username");
			return true;
		} else if (sPageName == "editnetwork") {
			CIRCNetwork* pNetwork = SafeGetNetworkFromParam(WebSock);

			// Admin||Self Check
			if (!spSession->IsAdmin() && (!spSession->GetUser() || !pNetwork || spSession->GetUser() != pNetwork->GetUser())) {
				return false;
			}

			if (!pNetwork) {
				WebSock.PrintErrorPage("No such username or network");
				return true;
			}

			return NetworkPage(WebSock, Tmpl, pNetwork->GetUser(), pNetwork);

		} else if (sPageName == "delnetwork") {
			CString sUser = WebSock.GetParam("user");
			if (sUser.empty() && !WebSock.IsPost()) {
				sUser = WebSock.GetParam("user", false);
			}

			CUser* pUser = CZNC::Get().FindUser(sUser);

			// Admin||Self Check
			if (!spSession->IsAdmin() && (!spSession->GetUser() || spSession->GetUser() != pUser)) {
				return false;
			}

			return DelNetwork(WebSock, pUser, Tmpl);
		} else if (sPageName == "editchan") {
			CIRCNetwork* pNetwork = SafeGetNetworkFromParam(WebSock);

			// Admin||Self Check
			if (!spSession->IsAdmin() && (!spSession->GetUser() || !pNetwork || spSession->GetUser() != pNetwork->GetUser())) {
				return false;
			}

			if (!pNetwork) {
				WebSock.PrintErrorPage("No such username or network");
				return true;
			}

			CString sChan = WebSock.GetParam("name");
			if(sChan.empty() && !WebSock.IsPost()) {
				sChan = WebSock.GetParam("name", false);
			}
			CChan* pChan = pNetwork->FindChan(sChan);
			if (!pChan) {
				WebSock.PrintErrorPage("No such channel");
				return true;
			}

			return ChanPage(WebSock, Tmpl, pNetwork, pChan);
		} else if (sPageName == "addchan") {
			CIRCNetwork* pNetwork = SafeGetNetworkFromParam(WebSock);

			// Admin||Self Check
			if (!spSession->IsAdmin() && (!spSession->GetUser() || !pNetwork || spSession->GetUser() != pNetwork->GetUser())) {
				return false;
			}

			if (pNetwork) {
				return ChanPage(WebSock, Tmpl, pNetwork);
			}

			WebSock.PrintErrorPage("No such username or network");
			return true;
		} else if (sPageName == "delchan") {
			CIRCNetwork* pNetwork = SafeGetNetworkFromParam(WebSock);

			// Admin||Self Check
			if (!spSession->IsAdmin() && (!spSession->GetUser() || !pNetwork || spSession->GetUser() != pNetwork->GetUser())) {
				return false;
			}

			if (pNetwork) {
				return DelChan(WebSock, pNetwork);
			}

			WebSock.PrintErrorPage("No such username or network");
			return true;
		} else if (sPageName == "deluser") {
			if (!spSession->IsAdmin()) {
				return false;
			}

			if (!WebSock.IsPost()) {
				// Show the "Are you sure?" page:

				CString sUser = WebSock.GetParam("user", false);
				CUser* pUser = CZNC::Get().FindUser(sUser);

				if (!pUser) {
					WebSock.PrintErrorPage("No such username");
					return true;
				}

				Tmpl.SetFile("del_user.tmpl");
				Tmpl["Username"] = sUser;
				return true;
			}

			// The "Are you sure?" page has been submitted with "Yes",
			// so we actually delete the user now:

			CString sUser = WebSock.GetParam("user");
			CUser* pUser = CZNC::Get().FindUser(sUser);

			if (pUser && pUser == spSession->GetUser()) {
				WebSock.PrintErrorPage("Please don't delete yourself, suicide is not the answer!");
				return true;
			} else if (CZNC::Get().DeleteUser(sUser)) {
				WebSock.Redirect(GetWebPath() + "listusers");
				return true;
			}

			WebSock.PrintErrorPage("No such username");
			return true;
		} else if (sPageName == "edituser") {
			CString sUserName = SafeGetUserNameParam(WebSock);
			CUser* pUser = CZNC::Get().FindUser(sUserName);

			if(!pUser) {
				if(sUserName.empty()) {
					pUser = spSession->GetUser();
				} // else: the "no such user" message will be printed.
			}

			// Admin||Self Check
			if (!spSession->IsAdmin() && (!spSession->GetUser() || spSession->GetUser() != pUser)) {
				return false;
			}

			if (pUser) {
				return UserPage(WebSock, Tmpl, pUser);
			}

			WebSock.PrintErrorPage("No such username");
			return true;
		} else if (sPageName == "listusers" && spSession->IsAdmin()) {
			return ListUsersPage(WebSock, Tmpl);
		} else if (sPageName == "traffic" && spSession->IsAdmin()) {
			return TrafficPage(WebSock, Tmpl);
		} else if (sPageName == "index") {
			return true;
		} else if (sPageName == "add_listener") {
			// Admin Check
			if (!spSession->IsAdmin()) {
				return false;
			}

			return AddListener(WebSock, Tmpl);
		} else if (sPageName == "del_listener") {
			// Admin Check
			if (!spSession->IsAdmin()) {
				return false;
			}

			return DelListener(WebSock, Tmpl);
		}

		return false;
	}