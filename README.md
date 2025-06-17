Historical source code for the **Rainlendar** project. The latest version can be found at http://www.rainlendar.net.

Rainlendar was initially hosted using *CVS* and not *Git*. I have not used Git's conversion tools for the CVS to Git migration, please refer to the headers at the top of the files for precise dates. Additionally, this repo does not maintain chronological order, as it consists of files I just unzipped and committed sequentially, please use tags to see the individual versions easier.

The original source files - including binaries and other tools - are available for download as a .tar archive in the **Releases** section.

# Version History
This section consists of the info inside the latest `Help\History.htm` file converted to markdown.

### 25 OCT 2005 (Version 0.22 ß)

*   Fixed timestamp issue with iCalendar events.
*   "All day event" wasn't sent to the server. Fixed.
*   The buttons in the event list are updated better now.
*   Event texts drawn into the calendar stay inside the day rectangle.
*   Different layouts can be used when multiple calendars are shown.
*   GridLeftToRight, GridTopToBottom, GridAcrossAndDown can be used to define how the months flow on the layouts.
*   New options for the skins: EventIconEnable, EventListItemMargin, EventListItemIconEnable
*   The separation between the days in the event list can be set with EventListDaySeparation.
*   Event list headers can use %D to display the number of days remaining to today.
*   %YN and %OT can be used to in event subject and message to display the number of years between now and start date of the event
*   New bang: !RainlendarShowTodaysEvent
*   The list windows can be hidden when there are no items with SmartTodoList and SmartEventList.
*   Birthday and Anniversary profiles use yearly recurrence by default.
*   Outlook message stores are identified by their ID instead of the name.
*   Snapping works also in the middle of the work area.
*   It's possible to use relative paths in the ini-format and ical plugins (the path must be added manually into the ini-file).
*   Images and buttons can be drawn on each calendar when multiple calendars are displayed.
*   RecurringPostfixString can be used in the profiles to add strings to the recurring events.
*   Time items can show world clock. Use Location in the items to define the place.
*   The time item can be shown in a balloontip/tooltip. The balloontip can be opened from the tray icon if there are no !bangs assigned to it.
*   Changed the way WeekNumberDelta is implemented. There numbering should be continuous now.

### 22 JUL 2005 (Version 0.21.2 ß)

*   All day events that lasted forever were not shown correctly in the all events dialog. Fixed
*   !RainlendarEditSkin didn't work. Fixed
*   The iCal-plugin hung if it tried to monitor a non-existing file. Fixed
*   Fixed a crash bug when events that were deleted were read from several sources.
*   Selecting events for editing from the context menu didn't work correctly. Fixed.
*   The "Until" in recurring events didn't work correctly. Fixed.
*   The window shouldn't popup on top anymore on midnight.
*   Alt-F4 is disabled if "Disable keyboard shortcuts" is checked. It also shouldn't react to unintended key presses so easily anymore.
*   iCal-plugin can read vCalendar files too.
*   Events can be imported just by dropping the file over any of the windows.
*   iCal-events are not imported as read-only anymore if the GUID cannot be decoded.
*   The iCal-plugin can now read also read-only files.
*   Fixed double clicking with vertical day layout.
*   Exporting iCal events over an existing file doesn't merge them anymore.
*   Double clicks on the event list window will always open the edit event dialog.
*   The ServerPlugin crashed if the message was too long. Fixed.
*   Fixed some endian problems with the server.
*   A "\\n" in the message is not converted to newline anymore.
*   Todo-list positions are stored also to the ics-files.
*   The alarm time is taken from the Outlook appointment.
*   Added NumOfComponents to the time item.
*   The parameter for !RainlendarShowNext and !RainlendarShowPrev didn't work. Fixed.

### 25 MAR 2005 (Version 0.21.1 ß)

*   Fixed event sorting again. Hopefully it now works correctly.
*   iCalendar plugin wrote also done todo-items to the ics-file. Fixed.
*   ToolTip used always the default font. Fixed.
*   Imported iCal events were sometimes a day too long. Fixed.
*   GrowUpwards didn't work correctly with the event list. Fixed.
*   Changed the file system monitoring in iCalPlugin. It now compares the filesize.
*   Fixed a memory leak in iCalPlugin.
*   "Refresh when screen resolution changes" works again.
*   Fixed time parsing routine.
*   "Mark all checked as done" doesn't affect read-only items anymore.
*   If the ServerPlugin was active modified events were not send to the server. Fixed.
*   Reversed the strings in month's and year's align combo box.
*   Fixed a crash bug in iCalendar and Server plugins.
*   Weekends can be defined with WeekendDays.

### 1 MAR 2005 (Version 0.21 ß)

*   Added support for iCalendar files.
*   Fixed a bug when importing old Rainlendar events that had time in the message left the time and removed the rest of the text even though it should have done exactly the opposite :-(
*   The plugin doesn't have to be active to be able to export events.
*   Fixed a crash bug when todo-files were removed from the IniFormatPlugin's list.
*   OutlookPlugin is not directly dependent from mapi32.dll anymore. This should get rid of those missing ordinal errors.
*   Fixed a problem where the end time was shown as one minute less than what it really was.
*   Added automatic polling to the ServerPlugin.
*   Recurrency -> Recurrence
*   All events list doesn't lose the selection when event is edited.
*   Monthly recurrence displayed sometimes 'fifth' instead of 'last' like it should have been. Fixed.
*   Seventh day of the month was displayed to belong to the second week in the monthly recurrence. Fixed.
*   Rainlendar now checks if it's already running before it starts. Use -m argument to disable the check.
*   Deleting items from the Done-list didn't work very well. Fixed.
*   The events in the message box are now sorted.
*   The "due in ..." text is not shown for the all day events and negative numbers are not shown for past events.
*   Editing todo-items when ServerPlugin was set as default crashed Rainlendar. Fixed.
*   Added fading effect. Use FadeDuration=0 to disable it.
*   Added "Opaque on mouse over" and "Transparent on mouse over" features.
*   Disabled possibility to open several instances of the dialogs.
*   Todo and events list can be made to grow upwards with GrowUpwards.
*   The hidden windows do not pop up anymore when "Hide on mouse over" is selected.
*   Sorting algorithm of the events now uses only the time and not the date.
*   It's possible to select which message stores are imported from Outlook.
*   Removed negative coords option from the config dialog (no point keeping it there).
*   Transparency settings won't apply to the message box; it is always opaque.
*   Stretched background is not included in the calendar size.
*   Fixed a crash bug with recurring events.
*   Changed the skin-format slightly (the old files still work too). It is now possible to define the drawing order.
*   Small adjustments to the OnDesktop-setting (the calendar shouldn't disappear so easily anymore).
*   The duration field is editable. Use m for minutes, h for hours and d for days. E.g. 90m for 90 minutes.
*   Added bangs: !RainlendarShowItem, !RainlendarHideItem, !RainlendarToggleItem, !RainlendarRedraw and !RainlendarChangeSkin.
*   Edit event dialog remembers the previously used profile again.
*   Added "Delete item" to the Todo-window's context menu.
*   !RainlendarQuit works in buttons.
*   The edit box for event message now wraps words instead of extends to the right.
*   Added ShowContextMenu option can be used to show only the context menu when right mouse button is pressed.
*   Added CalendarAction, TodoAction and EventListAction that can be used to define what happens when the window is double clicked.
*   Locked windows hide/show together.
*   It's possible to define the position of the message box with MessageBoxX and MessageBoxY.
*   The calendar quits from to Alt-F4.
*   Optimized text rendering. This should speed up refresh with multiple calendars.
*   EventListHeaderSeparation is included in header alignment rect. This might break some skins :(
*   Added HideFromCalendar and HideFromEventList which can be used to hide events with certain profile.
*   Multiday events show also the start and end date.
*   It's possible to define the color of the event with EventListFontColor in the profile section.
*   All day events can be longer than one day (there is no GUI to create multiday events though).
*   Keyboard override for click through and mouse hide can be disabled with DisableKeyboardOverride.
*   The profile priority is applied to the always drawn profiles too.
*   Added "visible"-argument to the !RainlendarToggle, which toggles only the visible windows.
*   "!RainlendarShow activate" only shows the calendar window.
*   The config editor can be changed with ConfigEditor (do not use any spaces in the path!)
*   Fonts use the default charset instead of the one defined in the skin. This can be overridden with UseSkinCharSet.

### 5 DEC 2004 (Version 0.20 ß)

*   New program icon by dieselx graphics (http://www.dieselx.de)
*   Fixed a negative coordinate problem with the tooltip.
*   Fixed a crash bug with Outlook support.
*   Message box shouldn't grow too large anymore.
*   Added "Get More Languages..." and "Get More Skins..." to the menu.
*   The order of todo-items can be changed.
*   Todo-list is multiselectable.
*   DisableDoneList=1 in Rainlendar.ini gets rid of the done-list.
*   Double-clicking on a todo-item will mark it done instead of opening the todo-dialog.
*   The plugins path can be defined with RainlendarPluginsPath in Litestep's step.rc.
*   Added Import and Export buttons to All Events dialog.
*   The windows can be enabled/disabled from the context menu.
*   Removed window coordinates from the config dialog.
*   Implemented an event list window which shows the coming events.
*   Added delete button to the all events dialog.
*   The columns in all events dialog are sortable.
*   Improved support for recurring events.
*   Skins can have custom text and date-items. See Skins-part in the docs for more info.
*   Skins can have buttons. See Skins-part in the docs for more info.
*   Windows snap to each other now too.
*   It is possible to lock window position to the calendar window.
*   Days can be set horizontally and vertically.
*   Weekends can be drawn with different font color/bitmap.
*   Added support for Outlook tasks.
*   Added a transparency slider to the config dialog.
*   Added click through feature (use Shift-key to disable it)
*   OnDesktop can be used with transparency.
*   !RainlendarShowEvents is gone.
*   Browsing for image files removes the absolute path automatically.
*   Small fixes to the usability of the skin editor.
*   It's possible to change the months with mouse wheel.
*   The todo-items can be synced with the server now too.
*   Weeknumbering can be adjusted with WeekNumberDelta.
*   Added "Don't show completed tasks" and "Don't show message body" to Outlook support.
*   Added some more hotkeys and !bangs.
*   Removed EventsPath-rcCommand (it doesn't work anymore).
*   Loading bmps don't create temp files anymore.
*   Changed the wallpaper polling routine a little.

### 9 MAY 2004 (Version 0.19.3 ß)

*   Refreshing the calendar lost the OnDesktop state. Fixed.
*   Rainlendar should work a bit better with Outlook 2000 now.
*   Fixed a crash bug with All events dialog.
*   Fixed a crash bug with Outlook sync.
*   Fixed !RainlendarShow activate. It only works when it's executed with Rainlendar.exe though,
*   Litestep variables can be used in the colors.
*   Rainlendar should now be able to access Outlook's appointments even if Outlook is not the default email client.
*   The todo list wasn't read from the Todo.ini-file unless it was enabled. Fixed.
*   Added DST calculations to Outlook dates, so they are not an hour off anymore (well, some still could be...).
*   Rainlendar is only logged in to MAPI when it's updating the Outlook appointments. Use OutlookKeepAlive=1 to keep the connection always on.
*   Fixed tooltip opening in wrong place.
*   The message box opened with wrong size sometimes. Fixed.

### 20 MAR 2004 (Version 0.19.2 ß)

*   If %APPDATA% is not defined Rainlendar writes the ini-files to it's own folder.
*   The messagebox has now correct size when it is opened the first time.
*   Rewrote the Outlook support to use the MAPI. This will get rid of the security warning with Outlook 2003.
*   Outlook's recurring events are better supported now.
*   Outlook profile can be selected with OutlookProfile in Rainlendar.ini.
*   Outlook appointment labels can be used as profiles with OutlookLabels=1 in Rainlendar.ini.
*   Fixed tooltip positioning in multimonitor systems.
*   Fixed some Win9x issues. The alphablending doesn't work correctly but at least it shouldn't crash anymore.

### 28 JAN 2004 (Version 0.19.1 ß)

*   If the Rainlendar.ini file was in the Rainlendar's folder a new file was created into Windows folder. Fixed.
*   Added internal handling for !Execute bang so that it can be used without Litestep too.
*   Refreshing doesn't pop up the todo window anymore if it is hidden.
*   Tray icon behaviour can be controlled with TrayExecuteL, TrayExecuteR, TrayExecuteM, TrayExecuteDL, TrayExecuteDM and TrayExecuteDR.

### 25 JAN 2004 (Version 0.19 ß)

*   The lists of languages and skins are now sorted.
*   It's not anymore possible to override general settings in the skin's ini-file.
*   Added a todo-list. It's a separate window, but most of the general settings apply also to it (movable, zposition etc).
*   Rewrote the window routines. Wharf/lsBox is not supported anymore :-(
*   All events list is now multiselectable and the read-only entries are drawn with gray color.
*   Changed image loading routine. JPEG-images should work now too and the wierd bug with vertical lines as days should be gone.
*   Outlook sync at startup is now optional.
*   Added control for setting the max tooltip width.
*   %n in execute will only use the 255 first characters in the text.
*   Replaced the standard message box with a skinnable one (LMB hides it, RMB snoozes).
*   Events are sorted by the start time.
*   Added a simple alarm system. Add time in front of the event message to define the time when the message box will appear. Time must be in HH:MM or HH:MM AM (or PM).
*   Added a trayicon (doubleclicking it toggles the calendar).
*   Improved the (un)installer. The installer can be now localized too.
*   Added context menu to the event editor.
*   It wasn't possible to set the event's valid until date to a day before the selected event. Fixed.
*   Manual can be opened from the context menu.
*   Added hotkey for the todo dialog.
*   Removed the warning messages with OnDesktop & Native transparency (they were quite annoying). The incompatible settings are now disabled.
*   "Execute on event" can play sounds now too. Just enter PLAY and the sound file. E.g. "PLAY boing.wav"
*   Event icons are drawn on top of the day numbers again.
*   The events, todos and settings are now written to the current user's application folder. This should make it easier to use Rainlendar in multiuser environment.
*   Added few new bangs: !RainlendarEditTodo, !RainlendarShowTodo, !RainlendarHideTodo, !RainlendarToggleTodo, !RainlendarAddEvent and !RainlendarShowEvents
*   The edit event dialog's position should be remembered now as well.
*   Month & year didn't restrict the window size. Fixed.

### 4 OCT 2003 (Version 0.18 ß)

*   It wasn't possible to add new events on days that already had repeating events. Fixed.
*   Improved the support for Outlook. It's still far from perfect though.
*   Added EventDrawAlways for profiles.
*   Added word wrapping to the tooltips. No GUI for it yet, but add ToolTipMaxWidth=500 to Rainlendar.ini to limit the tooltip window's width to 500 pixels.
*   The skin-list is now updated when the calendar is refreshed.
*   The Language-folders path defaults now to the folder where Rainlendar.dll is located.
*   Automatic refresh after suspend mode should work again.
*   Added priority system which defines what to display when a single day has several events.

### 23 AUG 2003 (Version 0.17.1 ß)

*   Months in the context menu and some other texts weren't translated. Fixed.
*   Some texts weren't translated at all or were wrong. Fixed.

### 9 AUG 2003 (Version 0.17 ß)

*   OnDestop radiobutton wasn't in the same group as other. Fixed.
*   Enlarged the dialogs a bit so that localized strings will fit better.
*   The DLL doesn't assume that its name is "Rainlendar.dll" anymore.
*   Newlines are removed from the event messages in the All Events -dialog.
*   Profiles and ordinals can be also localized.
*   The Outlook sync rate is now user definable and it can be also updated manually.
*   The window is not activated anymore during refresh (Note that Outlook sync does still activate the window).
*   Server synchronization got stuck if there were no events to be sent. Fixed.
*   Yearly Outlook appintments were displayed in every 12 years in Rainlendar. Fixed.
*   The current year can be changed with up and down keys.
*   Added support for XP styles.
*   Rewrote the Edit Event -dialog.
*   Double click on the All events -dialog allows editing of the selected event.
*   Added new option: Remember dialog positions.
*   Bottom right corner relative negative coordinates are now optional.
*   Native transparency is disabled if OnDesktop is selected and vice versa (they are incompatible settings).
*   Deleted events weren't propagated to other clients. Fixed.
*   Rainlendar shouldn't crash anymore if one of the images is missing.
*   The message box wasn't shown when day changed. Fixed.

### 15 JUN 2003 (Version 0.16 ß)

*   Negative y-coords didn't work. Fixed.
*   Added an option to show Outlook appointments in the calendar.
*   The background was clipped when native transparency was not used. Fixed.
*   "!RainlendarShow Activate" didn't work if executed with Rainlendar.exe. Fixed.
*   Added an option to start the week numbering from the week that contains January 1st.
*   Snapping should work now on all monitors.
*   Added option to sync with the server before adding/editing an event.
*   Added an error message if the server has a newer version of an event than the one sent to it.
*   The calendar is refreshed automatically after Windows returns from suspend state.
*   Switching on the native transparency while the calendar is set OnDesktop should work a bit better now.
*   The events for the same day are now sorted, so they should be always in the same order.
*   Added option to use separator line between events in the tooltip.
*   Added a dialog that shows all the events at the same time.
*   Added support for external language files for easier localization.
*   Stretching non-alpha backgrounds messed up the alpha channel. Fixed.
*   Added possiblity to show several months at the same time.

### 3 FEB 2003 (Version 0.15 ß)

*   Added !RainlendarMove and !RainlendarZPos bangs.
*   Fixed a bug that crashed the program if server name was empty.
*   Made the refresh on screen resolution optional, since it caused problems with some games.
*   The size of the today-image wasn't included in the window size calculation so part of the image was clipped in few skins. Fixed.

### 23 NOV 2002 (Version 0.14 ß)

*   The skins are loaded from the skins-subfolder by default.
*   The events path now defaults to the same folder where the Rainlendar.ini file is.
*   Fixed a bug that put wrong week number if the first day of the month was sunday.
*   Fixed a bug that sometimes caused multiplication of events when they were deleted.
*   Added support for icons in event profiles (check the shadow3-skin for examples).
*   Added PNG support for easier alpha transparency handling.
*   Improved the wallpaper polling routine; now it checks the timestamp too.
*   Added support for native Win2k/XP transparency.
*   Use Window Name -option was broken. Fixed.
*   Fixed a bug that caused a wrong day to be selected sometimes when the calendar was double clicked.
*   Added support for solid background with an optional bevel edge.
*   Added OnDesktop option to keep the calendar visible even with Explorer's "Show Desktop" feature.

### 24 AUG 2002 (Version 0.13 ß)

*   Messages are trimmed before they are sent to the server (to keep them below 1000 chars).
*   Removed mouse hooking code due to compatibility problems.
*   Added support for Litestep logging so it's easier to find bugs.
*   Fixed a bug that caused Litestep to crash after recycle.
*   Changing the months should be a bit faster now.
*   Added "Copy to Clipboard" button in the server config dialog.

### 10 AUG 2002 (Version 0.12 ß)

*   The window is now opened below all other windows when the position is set OnBottom.
*   Added an option to snap the window on the screen edges when it is moved.
*   Added !RainlendarLsBoxHook.
*   Fixed a bug that left the 'm' on the line when %m was used in the "Execute on event".
*   Added repeating events for daily, weekly, monthly and annual events.
*   Added profiles for event customization (check skinning.txt for more info).
*   Tooltip color & font can be changed from the skin settings.
*   !RainlendarShow can be executed with "activate" argument, which activates the window and puts it on top of other windows (doesn't work with OnBottom).

### 11 JUN 2002 (Version 0.11 ß)

*   Fixed a bug that set the dates to start from wrong day when the day changed.
*   Moving transparent window should be slightly faster now.
*   It was not possible to change the typeface of the fonts. Fixed.
*   Week numbers were not put in all lines that had days. Fixed.
*   Added possibility to set the separation (i.e. the space between) of the numbers in days, today, event, year and week numbers.
*   Separated the general settings and skin settings in different ini files. The skin settings override the general settings.
*   Now it is possible to change the skin from the context menu.
*   Negative coordinates are saved as zero to prevent the window to be placed off screen.
*   The GUI is back. You can once more change the skinning settings with it.
*   Added RainlendarSkinsPath step.rc command which defines the path for the skins.
*   Added RainlendarCurrentSkin and RainlendarCurrentSkinIni which will select the given skin.
*   Added !RainlendarEditSkin bang, which will open the skin editor dialog.

### 3 JUN 2002 (Version 0.10 ß)

*   Removed all MFC code from the Rainlendar.dll.
*   Added RainlendarEventsPath step.rc command.
*   The Rainlendar.ini file is only saved if it hasn't been changed by the user (this prevents overwriting manual modifications to the Rainlendar.ini).
*   Rainlendar is not anymore dependant on lsapi.dll. It only uses lsapi.dll's functions if ran as Litestep plugin.
*   Removed all skinning stuff from the GUI.

### 2 MAR 2002 (Version 0.9 ß)

*   Added support for distribution of Rainlendar's events. Check the Server part in the documentation for more info.
*   The calendar is automatically refreshed when the screen resolution changes.
*   Multimonitor functions are only used if the the OS supports them.
*   Fixed a bug with stretched wallpapers having noise pixels in the calendar window.
*   Now it is possible to use Rainlendar.exe to send the !bangs to the calendar (Only Rainlendar's !bangs work).
*   The items can be aligned a bit more freely on the calendar. This might affect existing configs, so check the Align-widget if something is in wrong place.
*   The background image can now be automatically stretched or tiled if it's too small for the calendar.
*   Added option to check if the name of the wallpaper has been changed since last refresh. And if it has, the calendar is automatically refreshed.
*   Added an About-dialog.
*   Rainlendar supports now Litestep's $evar$-variables
*   The calendar window can be moved with the mouse. Note that moving alphamasked calendar might cause problems with some gfx-cards.
*   Added support for OnTop, OnBottom and Normal positioning of the window.
*   Added support for transparency, which is based on window regions. Use magic pink color (FF00FF) in the background picture for transparent pixels (Note that you cannot use alphamask with the background).
*   Added an option to hide the window automatically when mouse is over the calendar (like in Rainmeter).

### 15 JAN 2002 (Version 0.8 ß)

*   Fixed a bug that prevented Rainlendar to find all its files if the command line argument was surrounded with quotes.
*   Changed the way the calendar is refreshed. Now it works even if there are other windows over it during the refreshing.
*   Now it is possible to define custom color/bitmap for different events.
*   Added possibility to show the event texts in the calendar window.
*   Fixed a memory leak during the refresh.
*   Added week numbers. They are calculated according to the ISO 8601-standard, so in countries that do not follow international standards (like in the U.S.) the week numbers might be incorrect.
*   The bitmap rasterized years are now placed in correct position.
*   By default Rainlendar.exe tries to load the configuration from a folder called _Default_. So, now it can be started without the need to give any arguments to the exe.
*   Removed the start delay option as it is quite unnecessary at the moment (let me know if someone wants it back :-).

### 26 DEC 2001 (Version 0.7 ß)

*   Now linked dynamically against MFC. You need might need to update your MFC42.DLL to run this.
*   Fixed small gfx-bugs in the dialogs.
*   The window name is now optional and can be disabled from the config-dialog.
*   The Quit button is grayed out if Rainlendar is ran as Litestep plug-in.
*   Added Wharf-interface so Rainlendar can be put inside a lsBox.

### 3 DEC 2000 (Version 0.6 ß)

*   Added new menu item to jump to the current month and a equivalent !bang for it (!RainlendarShowCurrent)
*   Added possibility disable hotkeys from the config.
*   Added hotkeys for changing the months. Note that key presses must be clean, i.e. modifiers (alt, shift, ...) cannot be used. This prevents accidental keypresses with vwm. Also note that that the calendar window must be active for hotkeys to work.
*   Rainlendar's window now has a name "Rainlendar"

### 2 OCT 2000 (Version 0.5 ß)

*   Fixed to work with the latest lsapi.dll's.
*   Rainlendar.exe now accepts quoted path as argument.
*   Fixed a bug that prevented the background image to display fully.

### 4 JULY 2000 (Version 0.4 ß)

*   Months can now cycled with cursor-keys.
*   Added few bangs: !RainlendarShow, !RainlendarHide, !RainlendarToggle, !RainlendarRefresh, !RainlendarConfig, !RainlendarQuit, !RainlendarShowNext, !RainlendarShowPrev, !RainlendarShowMonth \[Month\] \[Year\]
*   Added possibility to start the calendar hidden.
*   Changed the double buffering to full window version. This should reduce flickering.

### 13 MAY 2000 (Version 0.3 ß)

*   Changed the way how days are calculated.
*   Added possibility to display the current event in a MessageBox.
*   Added possibility execute a command (or !Bang) on a event-day. You can use %m for the event-message.
*   Added event days to display important dates. Doubleclick on a day to insert a text.
*   Added new tab in config-dialog to set the appearance of event days.
*   Alpha-Bitmaps can now have full path as well.
*   Displayed month can be selected form the context- menu.

### 6 APRIL 2000 (Version 0.2 ß)

*   Removed the Reload-menuitem.
*   RefreshDelay moved to the ini-file. There is no GUI for setting it, so you'll have to edit the ini-file by hand.
*   Month & weekday names are now in the ini-file for localization purposes.
*   Changed the way alpha-transparency is calculated.
*   Magic Pink works now for the background too.
*   Auto-refreshes when the day changes.

### 26 MARCH 2000 (Version 0.1 ß)

*   Initial Betarelease
