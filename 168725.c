static const char* rdpei_eventid_string(UINT16 event)
{
	switch (event)
	{
		case EVENTID_SC_READY:
			return "EVENTID_SC_READY";
		case EVENTID_CS_READY:
			return "EVENTID_CS_READY";
		case EVENTID_TOUCH:
			return "EVENTID_TOUCH";
		case EVENTID_SUSPEND_TOUCH:
			return "EVENTID_SUSPEND_TOUCH";
		case EVENTID_RESUME_TOUCH:
			return "EVENTID_RESUME_TOUCH";
		case EVENTID_DISMISS_HOVERING_CONTACT:
			return "EVENTID_DISMISS_HOVERING_CONTACT";
		default:
			return "EVENTID_UNKNOWN";
	}
}