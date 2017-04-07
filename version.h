#ifndef VERSION_H
#define VERSION_H

// Change this when you update things. It is used in Help->About.
#define QSTRIPPER_VERSION "1.10"

// Version History
//
// 1.01 - Added a new version numbering system.
//
//        Allowed PC Quill files to be opened.
//
// 1.02 - Corrected a bug whereby every file was being translated from
//        PC Quill format to QL Quill. This was a bad thing!
//
//        Moved the version number to its own header file.
//
// 1.03 - Extended the DOS to QL conversion table with details kindly
//        supplied by Geoff Wicks as a jpeg image. (Which has been
//        added to the Subversion repository.)
//
// 1.04 - Windows flatly refuses to show the Euro symbol. In Linux it
//        is $80/128 in Windows, the same, but shows up as a blob. And
//        yet, a Euro character is supposedly $80/128 in Windows too.
//
// 1.05 - Added main Window icon. It seemed to have the default one before.
//
// 1.06 - Added export to ODF facility. Now Quill Files can be exported to
//        Open Office and/or Libre Office.
//
// 1.07 - Fixed PC conversion of a umlaut. (132->228 as opposed to 132->246).
//
// 1.08 - Added ability to file->Open multiple files. (Derek Stewart)
//        After opening all files, the windows are cascaded to show them all. (Derek Stewart)
//        SourceForge URLs have been changed, by SourceForge, so fixed Help->About.
//
// 1.09 - Added commandline export options. (Derek Stewart)
//        Added Help (F1) option to explain the commandline stuff.
//        qstripper --help
//        qstripper --export --fmt list_of_files
//        fmt = --pdf, --docbook, --odf, --text or --html.
//        Exported files are created in the same folder as the input files.
//
// 1.10 - Rogue formatting bug (finally) Fixed. (Derek Stewart)
//        Caused by Quill not requiring a toggle off in the text for each toggle on. This
//        caused the current paragraph format to get out of sync as a new paragraph toggles
//        *all* formatting off. But the flags remained set. Sigh.


#endif // VERSION_H
