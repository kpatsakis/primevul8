	~SessionProtocolWorkingState() {
		free(environmentVariablesData);
	}