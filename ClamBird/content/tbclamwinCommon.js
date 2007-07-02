//-----------------------------------------------------------------------------
// Name:        ClamBirdCommon.js
// Product:     ClamWin Antivirus Thunderbird Extension
//
// Author(s):      budtse [budtse at users dot sourceforge dot net]
//
// Created:     2006/11/07
// Copyright:   Copyright ClamWin Pty Ltd (c) 2005-2006
// Licence:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA


const CL_CLEAN 		= 0;  // No virus found
const CL_VIRUS 		= 1;  // Virus found
const CL_EMAXREC	= 10  // recursion level limit exceeded
const CL_EMAXSIZE	= 11  // size limit exceeded
const CL_EMAXFILES	= 12  // files limit exceeded
const CL_NO_ACCESS 	= 56; // Can't open file
const CL_LOGGER_FAULT 	= 62; // Can't open file
const CL_ERAR		= 100 // rar handler error
const CL_EZIP		= 101 // zip handler error
const CL_EMALFZIP	= 102 // malformed zip
const CL_EGZIP		= 103 // gzip handler error
const CL_EBZIP		= 104 // bzip2 handler error
const CL_EOLE2		= 105 // OLE2 handler error
const CL_EMSCOMP	= 106 // compress.exe handler error
const CL_EMSCAB		= 107 // MS CAB module error
const CL_EACCES		= 200 // access denied
const CL_ENULLARG	= 300 // null argument error

const CL_ETMPFILE	= -1  // tmpfile() failed
const CL_EFSYNC		= -2  // fsync() failed
const CL_EMEM		= -3  // memory allocation error
const CL_EOPEN		= -4  // file open error
const CL_EMALFDB	= -5  // malformed database
const CL_EPATSHORT	= -6  // pattern too short
const CL_ETMPDIR	= -7  // mkdir() failed
const CL_ECVD		= -8  // not a CVD file (or broken)
const CL_ECVDEXTR	= -9  // CVD extraction failure
const CL_EMD5		= -10 // MD5 verification error
const CL_EDSIG		= -11 // digital signature verification error
const CL_EIO		= -12 // general I/O error
const CL_EFORMAT	= -13 // bad format or broken file
const CL_ESUPPORT	= -14 // not supported data format

const CL_EHWINIT	= -15 // hardware initialization failed
const CL_EHWLOAD	= -16 // error loading hardware database
const CL_EHWIO		= -17 // general hardware I/O error

const CL_EUSERABORT	= -50 // interrupted by callback



// Log a message to the JavaScript Console
var gConsoleService;		// the Java Console service (used for logging)
var gTempDir;			// path to the TEMP dir
var gTotalTimeout;		// Total timeout in milliseconds
var gDatabasePath;		// Path to the virus definition database
var gLogFilePath;			// Path to the log file
var gClamScanPath;
var nsClamwinService;


function getClamwinService() {
	if (nsClamwinService != null)
	{
		return nsClamwinService;
	}

	nsClamwinService =  Components.classes["@clamwin.com/clambird/nsClamWinService;1"];
	if (nsClamwinService == null)
	{
		LOG("nsClamwinService IS NULL!!!!");
	}
	nsClamwinService = nsClamwinService.getService();
	nsClamwinService = nsClamwinService.QueryInterface(Components.interfaces.nsIClamwinService);

	nsClamwinService.Initialize();

	nsClamwinService.Log("==================================================");

	return nsClamwinService;
}


function clambirdInitialize() {
	var comp = getClamwinService();
	if (comp == null)
		LOG ("CAN'T get nsClamWinService !!!");
}

function LOG(aMessage) {
	if (gConsoleService == null) {
		gConsoleService = Components.classes["@mozilla.org/consoleservice;1"]
                         .getService(Components.interfaces.nsIConsoleService);
	}


	if (gConsoleService == null) {
		return;
	}
	gConsoleService.logStringMessage(aMessage);
	var cw = getClamwinService();
	if (cw != null) {
		cw.Log(aMessage);
	}
}

function clambirdGetFrame(aWin, aFrameName) {
  LOG("clambirdCommon.js: clambirdGetFrame: name=" + aFrameName);
  for (var f = 0; f < aWin.frames.length; f++) {
    if (aWin.frames[f].name == aFrameName) {
		return aWin.frames[f];
    }
  }
  LOG("clambirdCommon.js: Frame " + aFrameName + " NOT FOUND!");
  return null;
}

function clambirdSaveAttachmentToTemp(aMessageUri, anAttachment) {

	// get a handle to the temporary folder
	var target = Components.classes["@mozilla.org/file/local;1"].
					createInstance(Components.interfaces.nsILocalFile);
	var comp = getClamwinService();
	if (comp == null)
		return null;

	target.followLinks = true;
	target.initWithPath(comp.GetTempFilename(anAttachment.displayName));

	// save the attachment to the folder
	LOG("clambirdSaveAttachmentToTemp: contentType = " + anAttachment.contentType);
	LOG("clambirdSaveAttachmentToTemp: url = " + anAttachment.url);
	LOG("clambirdSaveAttachmentToTemp: newName = " + target.leafName);
	LOG("clambirdSaveAttachmentToTemp: messageUri = " + aMessageUri);
	LOG("clambirdSaveAttachmentToTemp: target = " + target.parent.path);
	var savedFile = messenger.saveAttachmentToFolder(anAttachment.contentType,
							anAttachment.url,
							target.leafName,
							aMessageUri,
							target.parent);
	return savedFile;
}

function clambirdMoveToQuarantine(originalFile) {
	// get a handle to the quarantine folder
	var target = Components.classes["@mozilla.org/file/local;1"].
					createInstance(Components.interfaces.nsILocalFile);
	var comp = getClamwinService();
	if (comp == null)
		return null;

	target.followLinks = true;
	target.initWithPath(comp.GetQuarantineFilename(originalFile.leafName));

	originalFile.moveto(target.parent, target.leafName);
	return target;
}

function clambirdScanFile(aFile, reportFile) {
	var reportFile = {};
	var cw = getClamwinService();
	if (cw == null)
		return null;

	LOG("clambirdScanFile('" + aFile + "'");
	var ret = cw.ScanFile(aFile, reportFile);
	if (ret == 0)
	{
		return CL_FILESTATUS_CLEAR;
	}
	else
	{
		if (ret == CL_FILESTATUS_SAVING) {
			return CL_FILESTATUS_SAVING;
		}
		LOG("ReportFile : " + reportFile.value);
		return CL_FILESTATUS_INFECTED;
	}
}

function clambirdCloseDatabase() {
	var cw = getClamwinService();
	if (cw == null) {
		return;
	}
	cw.CloseDatabase();
}


function clambirdGetVersion() {
  var comp = getClamwinService();
  var version = comp.Version;
  LOG("clambirdGetVersion: Version = " + version);
  var tempFileName = comp.GetTempFilename("ClamWin1.log")
  LOG("clambirdGetTempFileName: = " + tempFileName);
}

function clambirdHideAttachments() {
	var node = document.getElementById("attachmentView");
	if (node) {
		node.hidden = true;
	}
}

function clambirdShowAttachments() {
	var node = document.getElementById("attachmentView");
	if (node) {
		node.hidden = false;
	}
}