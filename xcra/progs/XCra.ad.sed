! $Id: XCra.ad.sed,v 1.11 1996/08/04 15:37:50 gdraw Exp $
! Font
!
XCra*Text*font:			-adobe-helvetica-bold-r-normal-*-140-75-75-*-1
XCra*Label*font:		-adobe-helvetica-bold-o-normal-*-140-75-75-*-1
XCra*List*font:			9x15
XCra*List*forceColumns:		true
XCra*List*defaultColumns:	1

XCra*Form*resizable:		false

XCra*shapeStyle:		rectangle
XCra*cursor:			left_ptr

XCra*Label.height:		24
XCra*Label.justify:		left
XCra*Label.resize:		false
XCra*Label.width:		240

XCra*MenuButton.leftBitmap:	menu12
XCra*MenuButton.width:		150
XCra*MenuButton.resize:		false
XCra*MenuButton.left:		ChainLeft
XCra*MenuButton.right:		ChainLeft
XCra*MenuButton.shapeStyle:	rectangle

XCra*SimpleMenu.width:		170
XCra*SimpleMenu.cursor:		hand2

XCra*SmeBSB.HorizontalMargins:	10
XCra*SmeBSB.width:		140
XCra*SmeLine.width:		140

XCra*Box.orientation:		horizontal

XCra*Text.width:		100
XCra*Text.horizDistance:	10
XCra*Text.translations:		#override\n\
	<Btn1Down>:		select-tab-group() select-start()\n\
	<Key>Tab:		next-tab-group()\n\
	<Key>Return:		do-activate()\n\
	<Key>F1:		do-help()\n\
	<Key>F2:		do-close()

XCra*Form.translations:		#override\n\
	<Key>Return:		do-activate()\n\
	<Key>F1:		do-help()\n\
	<Key>F2:		do-close()

XCra*taskButton*cursor:		exchange
XCra*timesButton.fromHoriz:	taskButton
XCra*notesButton.fromHoriz:	timesButton
XCra*notesButton*label:		Notes

XCra*taskMenu*addTask*label:	Add task
XCra*taskMenu*delTask*label:	Remove task
XCra*taskMenu*refTask*label:	Update references
XCra*taskMenu*forTask*label:	Update forecasts
XCra*taskMenu*selTask*label:	Select task
XCra*taskMenu*swiTask*label:	Switch task

XCra*timesMenu*addTime*label:	Update time
XCra*timesMenu*repTime*label:	Report Form
XCra*timesMenu*incTime*label:	Get from
XCra*timesMenu*decTime*label:	Put to

XCra*notesMenu*addNote*label:	Add note
XCra*notesMenu*delNote*label:	Remove note
XCra*notesMenu*selNote*label:	Edit note
XCra*notesMenu*chgNote*label:	Select notes file
XCra*notesMenu*treeNote*label:	View note tree
XCra*notesMenu*grepNote*label:	Search in the notes

XCra*addTask.title:		Create Task
XCra*delTask.title:		Remove Task
XCra*ref.title:			Update Task References
XCra*forecasts.title:		Update Task Forecasts
XCra*listTask.title:		Select Task
XCra*switchTask.title:		Switch To Task

XCra*addTime.title:		Modify Time
XCra*report.title:		Report Time
XCra*decTime.title:		Add Time To Current Task
XCra*incTime.title:		Subtract Time From Current Task

XCra*selNoteFile.title:		Select Notes File

XCra*buttonForm*Command.width:	120
XCra*buttonForm*Command.vertDistance:	5
XCra*buttonForm*Command.shapeStyle:		oval
XCra*buttonForm*Command.height:	20

XCra*okButton.label:		Ok
XCra*okButton.cursor:		gumby

XCra*report*okButton.label:	Update Database

XCra*helpButton.label:		Help
XCra*helpButton.horizDistance:	20

XCra*cancelButton.label:	Cancel
XCra*cancelButton.horizDistance:20
XCra*cancelButton.cursor:	pirate


XCra*timeFormat:		%2tH:%02tM [%P]
XCra*taskFormat:		%N

XCra*cal*Label.width:		66
XCra*cal*Label.height:		18
XCra*cal*Label.resize:		false
XCra*cal*Label.justify:		center
XCra*cal*Command.height:	18
XCra*cal*Command.width:		66
XCra*cal*Command.resize:	false

XCra*cal.day2.label:	Lun
XCra*cal.day3.label:	Mar
XCra*cal.day4.label:	Mer
XCra*cal.day5.label:	Jeu
XCra*cal.day6.label:	Ven
XCra*cal.day7.label:	Sam
XCra*cal.day1.label:	Dim
XCra*cal*vertDistance:	2

XCra*cal.pMonth.bitmap:	%%XCRA_LIBDIR%%/larrow.xbm
XCra*cal.nMonth.bitmap:	%%XCRA_LIBDIR%%/rarrow.xbm
XCra*iconPixmap:	%%XCRA_LIBDIR%%/xcra.xbm
XCra*editNote*iconPixmap:	%%XCRA_LIBDIR%%/xnote.xbm
XCra*helpFile:		%%XCRA_LIBDIR%%/xcra.hlp

XCra*cal*month.width:	348

XCra*togtask.justify:	left
XCra*togtask.borderWidth:0
XCra*togtask.width:	400
XCra*togtask.resize:	false
XCra*togtask.height:	20
XCra*togtask.label:	%-20.20N %-10.10R %tH:%02tM
XCra*togtask.font:	9x15
XCra*togtask.bottom:	ChainTop
XCra*togtask.top:	ChainTop

XCra*labeltask.width:	360
XCra*labeltask.resize:	false
XCra*labeltask.height:	20
XCra*labeltask.label:	%-20.20N %-10.10R %tH:%02tM
XCra*labeltask.font:	9x15
XCra*labeltask.bottom:	ChainTop
XCra*labeltask.top:	ChainTop

XCra*ref*labeltask.label:	%-20.20N (%tH:%02tM)
XCra*forecasts*labeltask.label:	%-20.20N (%tH:%02tM)

XCra*Viewport.height:	200
XCra*Viewport.width:	500

XCra*allowHoriz:	false
XCra*allowVert:		false

XCra*task.top:		ChainTop
XCra*task.bottom:	ChainTop

XCra*report*form*Toggle.width:	95
XCra*report*form*labeltask.width:	450

XCra*togdayReport.label:	Day
XCra*togdayReport.state:	True
XCra*togweekReport.fromHoriz:	togdayReport
XCra*togweekReport.label:	Week
XCra*togfortReport.fromHoriz:	togweekReport
XCra*togfortReport.label:	Fortnight
XCra*togmonthReport.label:	Month
XCra*togmonthReport.fromHoriz:	togfortReport

XCra*tognameSort.label:		Names
XCra*tognameSort.state:		True
XCra*togdaySort.fromHoriz:	tognameSort
XCra*togdaySort.label:		Day Time
XCra*togweekSort.fromHoriz:	togdaySort
XCra*togweekSort.label:		Week Time
XCra*togfortSort.fromHoriz:	togweekSort
XCra*togfortSort.label:		Fortnight Time
XCra*togmonthSort.fromHoriz:	togfortSort
XCra*togmonthSort.label:	Month Time


XCra*labeltLabel.label:		Report the time for:
XCra*labelsLabel.label:		Sort the tasks on:

XCra*dayReportModel:		%-20.20N %-10.10R %2tH:%02tM
XCra*weekReportModel:		%-20.20N %-10.10R %2tH:%02tM %2wH:%02wM
XCra*fortnightReportModel:	%-20.20N %-10.10R %2tH:%02tM %2fH:%02fM
XCra*monthReportModel:		%-20.20N %-10.10R %2tH:%02tM %2aH:%02aM

XCra*labeltime.label:		Time value
XCra*labeltotalLabel.width:	300
XCra*labeltotalLabel.horizDistance:	200
XCra*labeltLabel.width:		300


XCra*addTask*Label*font:	-adobe-helvetica-bold-o-normal-*-140-75-75-*-1
XCra*addTask*Label.width:	140
XCra*addTask*Text.width:	240
XCra*addTask*labeltask.label:	Task name
XCra*addTask*labelrefName.label:Task reference
XCra*addTask*labelforcast.label:Forecast time
XCra*addTask*labeldate.label:	Creation date
XCra*addTask*labeladdTask.label:Imported Tasks
XCra*addTask*togtask.label:	%-20.20N %-10.10R

XCra*delTask*labeltask.label:	Task name

XCra*incTime*labeltask.label:	Time value
XCra*decTime*labeltask.label:	Time value
XCra*incTime*nameTitle.label:	From Task
XCra*decTime*labeltask.label:	Time value
XCra*decTime*nameTitle.label:	To Task

XCra*error.title:		Xcra Error Message
XCra*error*Text.width:		400
XCra*error*Text.height:		120
XCra*error*Text*font:		8x13
XCra*error.scrollVertical:	whenNeeded
XCra*error.scrollHorizontal:whenNeeded
XCra*error*okButton.label:	Dismiss

XCra*labeldecTime.label:	From Task                  Reference       Time
XCra*labeldecTime.width:	400
XCra*labelincTime.label:	To Task                  Reference       Time
XCra*labelincTime.width:	400
XCra*labellistTask.label:	Task		                              Reference
XCra*labellistTask.width:	400
XCra*labeldelTask.label:	Task		                              Reference       Time
XCra*labeldelTask.width:	400
XCra*labelref.label:		Task
XCra*labeladdTime.label:	Task			                 Reference       Time
XCra*labelreport.label:		Task			                 Reference       Time



XCra*labelnote.label:	Note title
XCra*labelfile.label:	Notes file
XCra*labelnLabel.label:	Note type
XCra*tognormNote.label:	Normal
XCra*tognormNote.fromHoriz:	labelnLabel
XCra*togextNote.label:	External
XCra*togextNote.fromHoriz:	tognormNote
XCra*togsubNote.label:	Notes File
XCra*togsubNote.fromHoriz:	togextNote

XCra*taskDirectory:	.cra

XCra*clock.width:	305
XCra*clock*font:	12x24

XCra*addNote*Text.width:	240
XCra*addNote*resizable:	true

XCra*editNote*Text.translations:#override\n\
	<Btn1Down>:		select-start()\n\
	<Key>Tab:		insert-char()\n\
	<Key>Return:		newline()\n\
	<Key>F1:		do-help()
XCra*editNote.note.width:	550
XCra*editNote.note.height:	300
XCra*editNote.note.scrollHorizontal:	whenNeeded
XCra*editNote.note.scrollVertical:	always
XCra*editNote.note*font:	9x15

XCra*nPrint.label:	Print
XCra*nDismiss.label:	Dismiss
XCra*nTitle.label:	Title
XCra*nClear.label:	Clear
XCra*nDestroy.label:	Destroy
XCra*nSelect.label:	Select

XCra*confirm.leftBitmap:Excl
XCra*confirm.width:	300
XCra*confirm.height:	50

XCra*clearNote*confirm.label:	Do you really want to clear this note ?
XCra*clearNote*cancelButton.horizDistance:100

XCra*createNoteFile*confirm.label:	The notes file does not exist.\n\
Do you want to create it ?
XCra*createNoteFile*cancelButton.horizDistance:100

XCra*destroyNote*confirm.label: Do you really want to delete this note ?
XCra*destroyNote*cancelButton.horizDistance:100

XCra*missTree*confirm.label:	This operation is not yet implemented.\n\
I'm sorry for this...


XCra*selNoteFile*Text.width:	240

XCra*delNote*cancelButton.horizDistance: 0
XCra*delNote*cancelButton.width: 240

XCra*addTime*cancelButton.label:	Done
XCra*addTime*okButton.label:		Modify
XCra*nSave.label:	Save
XCra*eyes.height:	40
XCra*eyes.width:	70
XCra*eyes.background:	black

XCra*result.width:	450
XCra*result.height:	400
XCra*result.scrollVertical:	always
XCra*result.scrollHorizontal:	whenNeeded
XCra*result*font:	8x13

XCra*labelregex.label:	Regular expression
XCra*grepNotes*file.width:	240
XCra*grepNotes*regex.width:	240
XCra*grepNotes.title:	Search in the notes
XCra*grepNotes*okButton:	Search


XCra*help*text.width:	560
XCra*help*text.height:	200
XCra*help*text.scrollVertical:	whenNeeded
XCra*help*text.scrollHorizontal:whenNeeded
XCra*help.title:	Oh Oh! Need some help?
XCra*help*Text*font:	8x13

XCra*help*cmdhelpTopic.label:	Previous topic
XCra*help*cmdhelpTopic.resize:	true
XCra*help*topics.width:	500
XCra*help*topics.resizable:	true

XCra*addNode.title:	Create Note
XCra*delNote.title:	Delete Note
XCra*selNote.title:	Select Note for Edition
XCra*clearNote.title:	Clear Note

!
! Layout 
!
XCra*FileSelector*Form.bottom:		Rubber

XCra*FileSelector*Text.right:		ChainRight
XCra*FileSelector*Text.left:		Rubber
XCra*FileSelector*Text.horizDistance:	30

XCra*FileSelector*Label.bottom:		ChainBottom
XCra*FileSelector*Label.right:		Rubber

XCra*FileSelector*title.titleLabel.bottom:	ChainBottom
XCra*FileSelector*title.titleLabel.right:	ChainRight

XCra*FileSelector*filter*Text.bottom:		ChainBottom

XCra*FileSelector*dirValue.bottom:		Rubber
XCra*FileSelector*dirName.bottom:		Rubber
XCra*FileSelector*fileName.top:		Rubber
XCra*FileSelector*fileValue.top:		Rubber
XCra*FileSelector*fileValue.bottom:		ChainBottom

XCra*FileSelector*fileValue.fromVert:		dirValue
XCra*FileSelector*fileValue.fromHoriz:	fileName
XCra*FileSelector*fileName.fromVert:		dirName
XCra*FileSelector*filterValue.fromHoriz:	filterName
XCra*FileSelector*dirValue.fromHoriz:		dirName
XCra*FileSelector*viewFiles.fromHoriz:	viewDirs
XCra*FileSelector*msgText.fromHoriz:		msgLabel

XCra*FileSelector*msgText.left:		Rubber
XCra*FileSelector*msgText.right:		ChainRight

XCra*FileSelector*msgText.horizDistance:	30

!
! Translations
!
XCra*FileSelector*dirValue.translations:	#override\n\
	<Btn1Down>:	select-tab-group() select-start()\n\
	<Key>Tab:	next-tab-group()\n\
	<Key>\\ :	Completion()\n\
	<Key>Return:	RescanDirectory()\n\
	<Key>F1:	do-help()\n\
	<Key>F2:	do-close()

XCra*FileSelector*fileValue.translations:	#override\n\
	<Btn1Down>:	select-tab-group() select-start()\n\
	<Key>Tab:	next-tab-group()\n\
	<Key>\\ :	Completion()\n\
	<Key>Return:	Select() do-activate()
	<Key>F1:	do-help()\n\
	<Key>F2:	do-close()

XCra*FileSelector*files.translations:	#override\n\
	<Btn1Up>(2):		Select() do-activate()\n\
	<Btn1Down>:		Set()\n\
	<Btn1Down>,<Btn1Up>:	Notify()\n\
	<Key>Return:		Select() do-activate()\n\
	<Key>\\ :		Set() Notify()

XCra*FileSelector*dirs.translations:	#override\n\
	<Key>Return:		Select()\n\
	<Key>\\ :		Set() Notify()\n\
	<Key>F1:	do-help()\n\
	<Key>F2:	do-close()

XCra*FileSelector*filterValue.translations:	#override\n\
	<Btn1Down>:	select-tab-group() select-start()\n\
	<Key>Tab:	next-tab-group()\n\
	<Key>Return:	RefilterList()\n\
	<Key>F1:	do-help()\n\
	<Key>F2:	do-close()

XCra*FileSelector*title.titleLabel.justify:	center

!
! Width and height of sub-widgets
!
XCra*FileSelector*Label.width:	80

XCra*FileSelector*Text.width:	300
XCra*FileSelector*Text.height:	22

XCra*FileSelector*List.height:	150
XCra*FileSelector*viewDirs.width:	202
XCra*FileSelector*viewFiles.width:	202

XCra*FileSelector*titleLabel*width:	410
XCra*FileSelector*msgText.width:	282
XCra*FileSelector*msgLabel.width:	100

!
! Label names
!
XCra*FileSelector*dirName.label:		Directory:
XCra*FileSelector*fileName.label:	File:
XCra*FileSelector*filterName.label:	Filter:
XCra*FileSelector*msgLabel.label:	Message:

XCra*FileSelector*List.resizable:	true
XCra*FileSelector*forceBars:		true

XCra*FileSelector*updateTime:		30
XCra*FileSelector*Form.borderWidth:	0

XCra*FileSelector*filterName:	*.notes
XCra*FileSelector*sortLists:	true
XCra*FileSelector*updateTime:	10
XCra*FileSelect*FileSelector*filterName:	*
