/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://amisproject.org

Copyright (c) 2008  DAISY Consortium

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef VOLMASTER_H
#define VOLMASTER_H

//Taken from and modified slightly:
// MSDN Magazine -- January 2004
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
// Compiles with Visual Studio .NET on Windows XP. Tab size=3.
//
namespace amis
{
namespace util
{
//Class to encapsulate Windows' logical disk drive functions.
class CVolumeMaster 
{
public:

  // Get bitmask of logical drives--same as Windows fn.
  DWORD GetLogicalDrives()
  {
    return ::GetLogicalDrives();
  }

  // Get drive type--same as Windows fn.
  UINT GetDriveType(LPCTSTR lpPath)
  {
    return ::GetDriveType(lpPath);
  }

  // Get logical drive strings as array of CStrings
  int GetLogicalDriveStrings(CStringArray& ar);

  // Get volume information using CStrings instead of LPTSTR
  BOOL GetVolumeInformation(LPCTSTR drive,   // eg, "\\Server\Share" or "C:\"
    CString& volname,            // volume name (label)
    DWORD& VolumeSerialNumber,       // volume serial number
    DWORD& MaximumComponentLength,   // maximum component len (name between \'s)
    DWORD& FileSystemFlags,        // flags
    CString& filesys);           // file system--eg FAT or NTFS

  // Handy helpers to get human-readable names of things
  static CString FormatDriveType(UINT type);
  static CString FormatBitMask(DWORD bitmap, int group=5);
  static CString FormatFileSystemFlags(DWORD flags, LPCTSTR sep);
};
}
}
#endif

