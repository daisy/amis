////////////////////////////////////////////////////////////////
// MSDN Magazine -- January 2004
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
// Compiles with Visual Studio .NET on Windows XP. Tab size=3.
//
#include "stdafx.h"
#include "util/VolMaster.h"

using namespace amis::util;

//////////////////
// Get logical drive strings as CStringArray
//
int CVolumeMaster::GetLogicalDriveStrings(CStringArray& ar)
{
	const int BUFLEN = 26*4;
	TCHAR buf[BUFLEN+1];
	::GetLogicalDriveStrings(BUFLEN, buf);

	for (LPCTSTR drive=buf; *drive; drive += _tcslen(drive)+1) {
		ar.Add(drive);
	}
	return ar.GetSize();
}

//////////////////
// Get logical drive info using CStrings instead of LPTSTR buffers.
//
BOOL CVolumeMaster::GetVolumeInformation(LPCTSTR drive,
	CString& volname,
	DWORD& VolumeSerialNumber,
	DWORD& MaximumComponentLength,
	DWORD& FileSystemFlags,
	CString& filesys)
{
	BOOL bRet= ::GetVolumeInformation(drive,
		volname.GetBuffer(_MAX_FNAME), _MAX_FNAME,
		&VolumeSerialNumber,
		&MaximumComponentLength,
		&FileSystemFlags,
		filesys.GetBuffer(_MAX_FNAME), _MAX_FNAME);
	volname.ReleaseBuffer();
	filesys.ReleaseBuffer();
	return bRet;
}

//////////////////
// Convert bitmask to string "0010011011..." etc.
// If group is specified, adds a space every group bits for legibility.
//
CString CVolumeMaster::FormatBitMask(DWORD bitmask, int group)
{
	CString bits;
	for (int i=0; i<32; i++) 
	{
		bits += (bitmask & 0x1) ? _T('1') : _T('0');
		if (group>0 && i%group+1 == group) bits += _T(' ');
		bitmask>>=1;
	}
	return bits;
}

///////////////////
// Get human-readable drive type.
//
CString CVolumeMaster::FormatDriveType(UINT type)
{
	
	const struct 
	{
		UINT type;
		LPCTSTR name;
	} TypeNames[] = 
	{
		{ DRIVE_UNKNOWN, _T("DRIVE_UNKNOWN") },
		{ DRIVE_NO_ROOT_DIR, _T("DRIVE_NO_ROOT_DIR") },
		{ DRIVE_REMOVABLE, _T("DRIVE_REMOVABLE") },
		{ DRIVE_FIXED, _T("DRIVE_FIXED") },
		{ DRIVE_REMOTE, _T("DRIVE_REMOTE") },
		{ DRIVE_CDROM, _T("DRIVE_CDROM") },
		{ DRIVE_RAMDISK, _T("DRIVE_RAMDISK") },
		{ 0, NULL },
	};
	for (int i=0; TypeNames[i].name; i++) 
	{
		if (type==TypeNames[i].type) return TypeNames[i].name;
	}
	CString str;
	str.Format(_T("DRIVE TYPE %d"),type);
	return str;
}

///////////////////
// Get human-readable FS flags, one per line.
// Adds sep (separator) between flags.
//
CString CVolumeMaster::FormatFileSystemFlags(DWORD flags, LPCTSTR sep)
{
/*	if (flags==0)
		return _T("[none]");

	const struct {
		long flag;
		LPCTSTR name;
	} FlagNames[] = {
		{ FILE_NAMED_STREAMS, _T("FILE_NAMED_STREAMS") },
		{ FILE_READ_ONLY_VOLUME, _T("FILE_READ_ONLY_VOLUME") },
		{ FILE_SUPPORTS_OBJECT_IDS, _T("FILE_SUPPORTS_OBJECT_IDS") },
		{ FILE_SUPPORTS_REPARSE_POINTS, _T("FILE_SUPPORTS_REPARSE_POINTS") },
		{ FILE_SUPPORTS_SPARSE_FILES, _T("FILE_SUPPORTS_SPARSE_FILES") },
		{ FILE_VOLUME_QUOTAS, _T("FILE_VOLUME_QUOTAS") },
		{ FS_CASE_IS_PRESERVED, _T("FS_CASE_IS_PRESERVED") },
		{ FS_CASE_SENSITIVE, _T("FS_CASE_SENSITIVE") },
		{ FS_FILE_COMPRESSION, _T("FS_FILE_COMPRESSION") },
		{ FS_FILE_ENCRYPTION, _T("FS_FILE_ENCRYPTION") },
		{ FS_PERSISTENT_ACLS, _T("FS_PERSISTENT_ACLS") },
		{ FS_UNICODE_STORED_ON_DISK, _T("FS_UNICODE_STORED_ON_DISK") },
		{ FS_VOL_IS_COMPRESSED, _T("FS_VOL_IS_COMPRESSED") },
		{ 0,NULL }
	};

	CString str;
	static TCHAR buf[256]={0};
	for (int i=0; FlagNames[i].name; i++) {
		if (flags & FlagNames[i].flag) {
			str += FlagNames[i].name;
			if (sep)
				str += sep;
		}
	}
	// remove last separator
	str.SetAt(str.GetLength()-_tcslen(sep),0);
	return str;
	*/

	//we don't use this function right now...no need to formalize this return value
	CString dummy = _T("dummy");
	return dummy;
}

