//// hd.cpp
//
//// hex-dump app
//
//// 12/07/00 (mv)
//// 05/18/02 (mv)  double-space option
//// 07/04/02 (mv)  .NET-ize, general updating
//
//#define _CRT_SECURE_NO_DEPRECATE
//#define _CRT_NONSTDC_NO_DEPRECATE
//
//typedef unsigned char BYTE;
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <tchar.h>
///*
//#define UNPRINTABLE	_T('.')
//
//FILE *fDump;
//FILE *out;
//int dblSpace;
//
//// translates a character if it is unprintable
//TCHAR GetChar(TCHAR _char)
//{
//	if (_char < 32)
//		return UNPRINTABLE;
//
//	return _char;
//}
//
//// opens the file to dump
//void OpenFile(TCHAR *filename)
//{
//	fDump = _tfopen(filename, _T("rb"));
//	if (!fDump)
//	{
//		_tprintf(_T("Error opening \"%s\"\n"), filename);
//		exit(1);
//	}
//}
//
//// dumps the file
//void DumpFile(TCHAR *filename)
//{
//	// nSpaces solves the problem when a line is incomplete.
//	// The number of spaces between the start of the hex data
//	// and the ASCII text is stored here - if the file does
//	// not end on a 16-bit boundary, the number here will be
//	// the number of spaces to add so the ASCII column will
//	// be aligned properly.
//
//	BYTE fChar;
//	int i=0;
//	long bytesRead=0;
//	TCHAR line[17];
//	int nSpaces=3*16+2;			// spaces from beg. of data
//	memset(&line, 0, sizeof(line));
//
//	// print the filename
//	_tprintf(_T("%s\n\n"), filename);
//
//	if (dblSpace)
//		_tprintf(_T("\n\n"));
//
//	for (;;)
//	{
//		// print hex address
//		_tprintf(_T("%08X  "), i);
//
//		// print first 8 bytes
//		for (int j = 0; j < 0x08; j++)
//		{
//			fChar = (BYTE)fgetc(fDump);
//			if (feof(fDump))
//				break;
//
//			_tprintf(_T("%02X "), fChar);
//
//			// add to the ASCII text
//			line[bytesRead++] = GetChar(fChar);
//
//			// this took three characters
//			nSpaces -= 3;
//		}
//
//		// print last 8 bytes - change in the "xx " to " xx" provides
//		// the double space in between the first 8 and the last 8 bytes.
//		for (int j = 0x08; j < 0x10; j++)
//		{
//			fChar = (BYTE)fgetc(fDump);
//			if (feof(fDump))
//				break;
//
//			_tprintf(_T(" %02X"), (unsigned char)fChar);
//
//			// add to the ASCII text
//			line[bytesRead++] = GetChar(fChar);
//
//			// this took three characters
//			nSpaces -= 3;
//		}
//
//		// fill in any leftover spaces.
//		for (int j = 0; j <= nSpaces; j++)
//		{
//			_tprintf(_T(" "));
//		}
//
//		// print ASCII text
//		_tprintf(_T("%s"), line);
//
//		// quit if the file is done
//		if (feof(fDump))
//			break;
//
//		// new line
//		_tprintf(_T("\n"));
//
//		if (dblSpace)
//			_tprintf(_T("\n"));
//
//		// reset everything
//		bytesRead=0;
//		memset(&line, 0, sizeof(line));
//		i += 16;
//		nSpaces = 3*16+2;
//	}
//}
//
//// closes everything
//void CloseFile()
//{
//	_tprintf(_T("\n"));
//	fclose(fDump);
//}
//*/
//#include <array>
//#include <set>
//#include <vector>
//#include <chrono>
//#include <time.h>
//#include <Windows.h>
//extern "C" int _tmain(int argc, TCHAR **argv)
//{
//
//    std::array<int, 10> arr;
//    arr[0] = 123;
//	// usage: hd <filename> [options]
//	/*if (argc == 1)	// no args
//	{
//		_tprintf(_T("Michael's HexDump Application version 1.6\n"));
//		_tprintf(_T("   Usage:  hd [/d] <filename>\n\n"));
//		_tprintf(_T("Options:\n"));
//		_tprintf(_T("   /d       double-space output\n"));
//		_tprintf(_T("\n"));
//
//		return;
//	}
//
//	dblSpace = 0;
//
//	if (argc == 3)
//	{
//		if (!_tcsicmp(argv[1], _T("/d")))
//			dblSpace = 1;
//	}
//
//	OpenFile(argv[argc-1]);
//	DumpFile(argv[argc-1]);
//	CloseFile();*/
//
//
//    int *heap = new int[20];
//    heap[8] = 1793;
//
//    //time_t seconds;
//
//    //seconds = time(NULL);
//
//
//    return GetTickCount();
//}
