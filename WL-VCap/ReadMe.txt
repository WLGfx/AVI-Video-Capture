Using http://msdn.microsoft.com/en-us/library/ms633497(VS.85).aspx
I can get the list of windows active on the desktop.
This can be useful for selecting which window to record the video from.

EDIT: Now I'm having to switch to EnumProcesses() instead to retrieve the
list of running processes instead of apps.

/////////////////////////////////////////////////////////////////////////////

1. Initialise program startup

2. Assign global dialog class across all cpp file

3. Initialise dialogs first run
	a. Enumerate application windows
	b. Add windows title to the list box
	c. Filter out non active windows

4. Activate the fileselector

5. Start button
	a. Remove old file

st helens cardio - fri apr 20 1:30pm

Cardiology Appt @ St Helens Hospital - Fri Apr 20th @ 1:30pm