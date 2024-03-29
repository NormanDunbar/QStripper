#ifndef VERSION_H
#define VERSION_H

// Change this when you update things. It is used in Help->About.
#define QSTRIPPER_VERSION "1.17"

// Version History
// 1.17 - Credited Cristian for his 'background.jpg' image aka QL 2001. Also
//        fixed duplicate shortcut CTRL+SHIFT+R which exports RST and ASC.
//        Now uses CTRL+SHIFT+A for ASC exports.
//        Fixed a pile of C++17 Warnings.
//        Removed a lot of commented out code, it's not used.
//
// 1.16 - Fulfilled a request to have background images configurable, and
//        would like Saturn rather than Jupiter. I did my best! (Derek Stewart)
//
// 1.15 - Some idiot programmer, ie, me, forgot to allow --rst and --asc as
//        valid export formats when using command line QStripper. This has now
//        been fixed.
//
// 1.14 - Added export in ASCIIDOC[tor] format. This also fixed a weird problem
//        where there was a set of minimise, maximise and close buttons on the
//        far left of the application's display. I didn't do anything to fix this
//        by the way, they just vanished as mysteriously as they arrived.
//
// 1.13 - FINALLY!!! Fixed that damned Euro! It now displays in the editor and exports!
//        Added a long needed "recent files" option to the file menu. The last 10
//        opened files will be found here - assuming that they still exist of course.
//        They can be opened with CTRL + 1 through CTRL + 0 - if you remember the order!
//
//
// 1.12 - Added automatic scroll bars to the workspace so that if an opened file
//        hangs over the main window border, it can be scrolled to, if necessary.
//        Added Jupiter background. Just because!
//        Settings now stored in "Dunbar IT Consultants Ltd" and not in "Dunbar-it"
//        to be consitent with other utilities.
//
// 1.11 - Only cascade open files when more than one is opened on the same "open"
//        call.
//
//        Enforce a minimum size for the main window (640,480) and any open document
//        (400, 250) within.
//
//        Attempt to replicate TABs from the Quill Document - not as easy as it should be!
//
//        Removed the yellow background added at 1.10 as it replicates in the HTML and PDF
//        exports. Not an ideal situation. Added to main window as that doesn't affect outputs.
//
//        Text export is now encoded in UTF-8 as we need this for accented European chracters.
//
//        Bug fix. When document was modified and then exported, you were still prompted
//        that it had changed and did you want to export it. Duh!
//
// 1.10 - Rogue formatting bug (finally) Fixed. (Derek Stewart)
//        Caused by Quill not requiring a toggle off in the text for each toggle on. This
//        caused the current paragraph format to get out of sync as a new paragraph toggles
//        *all* formatting off. But the flags remained set. Sigh.
//
// 1.09 - Added commandline export options. (Derek Stewart)
//        Added Help (F1) option to explain the commandline stuff.
//        qstripper --help
//        qstripper --export --fmt list_of_files
//        fmt = --pdf, --docbook, --odf, --text or --html.
//        Exported files are created in the same folder as the input files.
//
// 1.08 - Added ability to file->Open multiple files. (Derek Stewart)
//        After opening all files, the windows are cascaded to show them all. (Derek Stewart)
//        SourceForge URLs have been changed, by SourceForge, so fixed Help->About.
//
// 1.07 - Fixed PC conversion of a umlaut. (132->228 as opposed to 132->246).
//
// 1.06 - Added export to ODF facility. Now Quill Files can be exported to
//        Open Office and/or Libre Office.
//
// 1.05 - Added main Window icon. It seemed to have the default one before.
//
// 1.04 - Windows flatly refuses to show the Euro symbol. In Linux it
//        is $80/128 in Windows, the same, but shows up as a blob. And
//        yet, a Euro character is supposedly $80/128 in Windows too.
//
// 1.03 - Extended the DOS to QL conversion table with details kindly
//        supplied by Geoff Wicks as a jpeg image. (Which has been
//        added to the Subversion repository.)
//
// 1.02 - Corrected a bug whereby every file was being translated from
//        PC Quill format to QL Quill. This was a bad thing!
//
//        Moved the version number to its own header file.
//
// 1.01 - Added a new version numbering system.
//
//        Allowed PC Quill files to be opened.

#endif // VERSION_H
