//-----------------------------------------------------------------------------
// Name:        ClamBirdMsgHdrViewOverlay.js
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

const CL_FILESTATUS_SAVING 	= 20000;	// File is being saved
const CL_FILESTATUS_SCANNING 	= 20001;	// File is being scanned
const CL_FILESTATUS_CLEAR 		= 20003;	// File was scanned and not infected
const CL_FILESTATUS_INFECTED 	= 20004;	// File was scanned and infected


var gTempFiles = [];		// Array holding the temporary files
var gTempFilesStatus = [];	// Array holding the statuses
var gProcessStartTime;		// Date object holding time the process started
var gMessageUri;			// Holds the URI of the selected message

var clambird = {

  onLoad: function() {
    LOG("clambirdMsgHeaderViewOverlay: clambird.onLoad");
    // initialization code
    this.initialized = true;
    this.strings = document.getElementById("clambird-strings");
    LOG("clambird version : " + clambirdGetVersion());
  },

  onMenuItemCommand: function(e) {
    LOG("clambirdMsgHeaderViewOverlay: onMenuItemCommand");
    var promptService = Components.classes["@mozilla.org/embedcomp/prompt-service;1"]
                                  .getService(Components.interfaces.nsIPromptService);
    promptService.alert(window, this.strings.getString("helloMessageTitle"),
                                this.strings.getString("helloMessage"));
  },

  onMsgHdrViewHide: function() {
    LOG("clambirdMsgHeaderViewOverlay: onMsgHdrViewHide");
  },

  onMsgHdrViewUnhide: function() {
    LOG("clambirdMsgHeaderViewOverlay: onMsgHdrViewUnhide");
  },

  onMsgHdrViewLoad: function() {
    LOG("clambirdMsgHeaderViewOverlay: onMsgHdrViewLoad");

    // register more listeners
    var listener = {};
    listener.onStartHeaders = clambird.onStartHeaders;
    listener.onEndHeaders = clambird.onEndHeaders;
    listener.beforeStartHeaders = clambird.onBeforeStartHeaders;
    gMessageListeners.push(listener);
  },

  onBeforeStartHeaders: function() {
    LOG("clambirdMsgHeaderViewOverlay: onBeforeStartHeaders");
  },

  onStartHeaders: function() {
    LOG("clambirdMsgHeaderViewOverlay: onStartHeaders");

    var msgFrame = clambirdGetFrame(window, "messagepane");
    if (msgFrame) {
      LOG("clambirdMsgHdrViewOverlay.js: msgFrame=" + msgFrame);

      msgFrame.addEventListener("load", clambird.onMessageLoad, true);
      msgFrame.addEventListener("unload", clambird.onMessageUnload, true);
    }
  },

  onEndHeaders: function() {
    LOG("clambirdMsgHeaderViewOverlay: onEndHeaders");
  },

  onMessageLoad: function() {
    LOG("clambirdMsgHeaderViewOverlay: onMessageLoad");

    var savedFiles;    // an array to hold all files saved
    var hasAttachments = currentAttachments && currentAttachments.length;


	gMessageUri = GetFirstSelectedMessage();

    // Save all attachments to the temporary folder
    if (hasAttachments) {
		clambirdHideAttachments();
		for (var indexb in currentAttachments) {
		  var attachment = currentAttachments[indexb];
		  LOG("Attachment (" + indexb + "): " + attachment.displayName + ", " + attachment.url);

		  // save the attachment to the temporary directory
		  var savedFile = clambirdSaveAttachmentToTemp(gMessageUri, attachment);
		  if (savedFile == null) {
			LOG("clambirdMsgHdrViewOverlay.js: onMessageLoad : ERROR saving file to TempDir!");
		  }
		  else
		  {
			gTempFiles.push(savedFile);
			gTempFilesStatus.push(CL_FILESTATUS_SAVING);
		  }

		}

		// Remember current time
		gProcessStartTime = new Date();

		// Wait until all files exist
		setTimeout(clambird.CheckFiles, 100);
	}
  },

  onMessageUnload: function() {
    LOG("clambirdMsgHeaderViewOverlay: onMessageUnload");

    // Delete the temporary files
    if (gTempFiles.length > 0) {
      for (var f=0; f < gTempFiles.length; f++) {
		  if (gTempFiles[f].exists) {
			gTempFiles[f].remove(true);
		  }
      }
    }

    // Clear the tempFiles array
    while (gTempFiles.length > 0) {
      gTempFiles.pop();
    }

    // Clear the tempFilesStatus array
    while (gTempFilesStatus.length > 0) {
      gTempFilesStatus.pop();
    }
  },

	CheckFiles: function() {
		LOG("clambird.CheckFiles");

		var allDone = true;

		// Check general timeout is reached
		var currentTime = new Date();
		var elapsedTime = currentTime - gProcessStartTime;
		var reportFile;
		if (elapsedTime > gTotalTimeout) {
			LOG("TIMEOUT processing files");
			// Timeout elapsed, show a message box and quit
			// TODO
			return;
		}

		if (gTempFiles.length > 0) {
			for (var f=0; f < gTempFiles.length; f++) {
				switch(gTempFilesStatus[f]) {
					case CL_FILESTATUS_SAVING: {
						// Start scanning this file
						LOG("Trying to scan '" + gTempFiles[f].path + "', status = " + gTempFilesStatus[f]);
						gTempFilesStatus[f] = clambirdScanFile(gTempFiles[f].path, reportFile);
						LOG("clambirdScanFile return code = " + gTempFilesStatus[f]);
						if (gTempFilesStatus[f] == CL_FILESTATUS_SAVING) {
							LOG("File '" + gTempFiles[f].path + "' is still saving");
							allDone = false;
							return;
						}
						if (gTempFilesStatus[f] == CL_FILESTATUS_CLEAR) {
							LOG("File is clean");
						} else {
							LOG("File is infected");
							LOG("ReportFile = " + reportFile);
							// Remove the attachment from this email

							// For now, just show a messagebox
                            alert("ClamWin detected a virus in attachment " + gTempFiles[f].leafName);

						}
						break;
					}
					default: {
					}
				}  // switch
			} // for
		} // gTempFiles.length > 0

		if (allDone) {
			LOG("All files are scanned!");
			clambirdCloseDatabase();

			// The mail can now be shown in the reading pane
			clambirdShowAttachments();

			return;
		}

		// not all files were saved yet, so
		// wait for some time, then call this function again until all done
		setTimeout(clambird.CheckFiles, 100);
	}
};


// Initialize
clambirdInitialize();

// Add the eventlistener for the onLoad function
window.addEventListener("load", function(e) { clambird.onLoad(e); }, false);

window.addEventListener('messagepane-loaded', clambird.onMsgHdrViewLoad, true);
window.addEventListener('messagepane-hide', clambird.onMsgHdrViewHide, true);
window.addEventListener('messagepane-unhide', clambird.onMsgHdrViewUnhide, true);

