// 
// General header file for TBV plugin support
//

#ifndef TBVPLUGININTERFACE_H
#define TBVPLUGININTERFACE_H

// #include "Plugin_FMR_Header.h"
// #include "Plugin_VMR_Header.h"

#include "global.h" // to get platform (OS) definitions (plus macros, qxRound())
#include <QtPlugin>	// SB: Qt

// make sure that "MAKE_PLUGIN" is defined here or as a preprocessor definition (this is only important on Windows)
#define MAKE_PLUGIN

#if defined OS_WIN32
	#ifdef MAKE_PLUGIN
		#define PLUGIN_ACCESS __declspec(dllexport)
	#else
		#define PLUGIN_ACCESS __declspec(dllimport)
	#endif
#else
	// for all other platforms PLUGIN_ACCESS is empty (not needed)
	#define PLUGIN_ACCESS
#endif

#if defined(OS_WIN32)
	#include <windows.h>
	#include <malloc.h>
#endif
#include <stdio.h>
#include <stdlib.h>

#ifndef OS_WIN32
	#include <dlfcn.h>
#endif

#ifdef OS_WIN32
	#define GETSYMADDRESS GetProcAddress
#else
	#define GETSYMADDRESS dlsym
#endif

class PLUGIN_ACCESS TBVPluginInterface 
{

public:	 
	TBVPluginInterface() {};
	virtual ~TBVPluginInterface() {};
	
	// this function will be called once
	virtual bool initPlugin() = 0;
	// these methods will be called to execute the plugin (your essential code)
	// the "executePre/PostRun()" functions will be called once while the
	// executePre/PostStep()" functions will be called at each incremental processing step
	virtual bool executePreRun() = 0;
	virtual bool executePreStep() = 0;
	virtual bool executePostStep() = 0;
	virtual bool executePostRun() = 0;

};


// These are typedef's for dynamic access of TBV functions - version with "static" definitions
// Commands start with "t" to indicate that these are TurboBrainVoyager access functions

// general functions
typedef void (*GetVersionOfTBVPrototype)(int *, int *, int *);
static GetVersionOfTBVPrototype tGetVersionOfTBV;

typedef void (*GetTBVPathPrototype)(char *);
static GetTBVPathPrototype tGetTBVPath;

typedef void (*GetPluginsPathPrototype)(char *);
static GetPluginsPathPrototype tGetPluginsPath;


// output in log window
typedef void (*LogTextPrototype)(const char *);
static LogTextPrototype tLogText;

// dialogs to get/provide information - note these fns are only usable in non-incremental calls (ignored in "execute()" plugin fn)
typedef void (*MessageBoxPrototype)(const char *);
static MessageBoxPrototype tMessageBox;

typedef int (*YesNoMessageBoxPrototype)(const char *);
static YesNoMessageBoxPrototype tYesNoMessageBox;

typedef int (*GetIntegerInputPrototype)(const char *, int, int, int);
static GetIntegerInputPrototype tGetIntegerInput;

typedef float (*GetFloatInputPrototype)(const char *, float, float, float);
static GetFloatInputPrototype tGetFloatInput;

typedef int (*GetTextInputPrototype)(const char *, const char *, char *);
static GetTextInputPrototype tGetTextInput;

typedef int (*GetOpenFileNamePrototype)(const char *, const char *, const char *, char *);
static GetOpenFileNamePrototype tGetOpenFileName;

typedef int (*GetSaveFileNamePrototype)(const char *, const char *, const char *, char *);
static GetSaveFileNamePrototype tGetSaveFileName;


// TBV info
typedef int (*GetCurrentTimePointPrototype)();
static GetCurrentTimePointPrototype tGetCurrentTimePoint;

typedef int (*GetExpectedNrOfTimePointsPrototype)();
static GetExpectedNrOfTimePointsPrototype tGetExpectedNrOfTimePoints;

typedef void (*GetDimsOfFunctionalDataPrototype)(int &, int &, int &);
static GetDimsOfFunctionalDataPrototype tGetDimsOfFunctionalData;

typedef void (*GetProjectNamePrototype)(char *);
static GetProjectNamePrototype tGetProjectName;

typedef void (*GetWatchFolderPrototype)(char *);
static GetWatchFolderPrototype tGetWatchFolder;

typedef void (*GetTargetFolderPrototype)(char *);
static GetTargetFolderPrototype tGetTargetFolder;

typedef void (*GetFeedbackFolderPrototype)(char *);
static GetFeedbackFolderPrototype tGetFeedbackFolder;

typedef int (*GetCurrentProtocolConditionPrototype)();
static GetCurrentProtocolConditionPrototype tGetCurrentProtocolCondition;

typedef int (*GetFullNrOfPredictorsPrototype)();
static GetFullNrOfPredictorsPrototype tGetFullNrOfPredictors;

typedef int (*GetCurrentNrOfPredictorsPrototype)();
static GetCurrentNrOfPredictorsPrototype tGetCurrentNrOfPredictors;

typedef int (*GetNrOfConfoundPredictorsPrototype)();
static GetNrOfConfoundPredictorsPrototype tGetNrOfConfoundPredictors;

typedef float (*GetValueOfDesignMatrixPrototype)(int, int);
static GetValueOfDesignMatrixPrototype tGetValueOfDesignMatrix;

typedef int (*GetNrOfContrastsPrototype)();
static GetNrOfContrastsPrototype tGetNrOfContrasts;

typedef int (*GetNrOfROIsPrototype)();
static GetNrOfROIsPrototype tGetNrOfROIs;

typedef float (*GetMeanOfROIPrototype)(int);
static GetMeanOfROIPrototype tGetMeanOfROI;

typedef int (*GetNrOfVoxelsOfROIPrototype)(int);
static GetNrOfVoxelsOfROIPrototype tGetNrOfVoxelsOfROI;

typedef float (*GetBetaOfROIPrototype)(int, int);
static GetBetaOfROIPrototype tGetBetaOfROI;

typedef bool (*GetCoordsOfVoxelOfROIPrototype)(int, int, int &, int &, int &);
static GetCoordsOfVoxelOfROIPrototype tGetCoordsOfVoxelOfROI;

typedef float (*GetValueOfVoxelAtTimePrototype)(int, int, int, int);
static GetValueOfVoxelAtTimePrototype tGetValueOfVoxelAtTime;

typedef short int **(*GetTimeCourseDataPrototype)();
static GetTimeCourseDataPrototype tGetTimeCourseData;

typedef double (*GetBetaOfVoxelPrototype)(int, int, int, int);
static GetBetaOfVoxelPrototype tGetBetaOfVoxel;

typedef double *(*GetBetaMapsPrototype)();
static GetBetaMapsPrototype tGetBetaMaps;

typedef float (*GetMapValueOfVoxelPrototype)(int, int, int, int);
static GetMapValueOfVoxelPrototype tGetMapValueOfVoxel;

typedef float *(*GetContrastMapsPrototype)();
static GetContrastMapsPrototype tGetContrastMaps;


// Locally defined functions
//
inline bool InitTBVAccess()
{
#ifdef OS_WIN32

	HMODULE hndl = GetModuleHandleA("TurboBrainVoyager.exe");

	if(hndl == 0)
		return false;

#else

	// handle Mac and Linux
	
	void *hndl = dlopen(NULL, RTLD_LAZY);  // NULL pointer to get handle to main process
	if(hndl == NULL)
		return false;

#endif

	//
	// general functions
	//
	tGetVersionOfTBV = (GetVersionOfTBVPrototype)GETSYMADDRESS( hndl, "tGetVersionOfTBV" );
	if(tGetVersionOfTBV == NULL) return false;

	// IMPORTANT: If we do not get a pointer, we do not return false
	//  To use specific commands, a) you must check the obtained version string and use only those commands available in that version
	//                         or b) Check whether the function pointers are non-NULL for each command you want to use in your code

	tGetTBVPath = (GetTBVPathPrototype)GETSYMADDRESS( hndl, "tGetTBVPath" );
	tGetPluginsPath = (GetPluginsPathPrototype)GETSYMADDRESS( hndl, "tGetPluginsPath" );

	// input and display (GUI) functions
	//
	tLogText = (LogTextPrototype)GETSYMADDRESS( hndl, "tLogText" );
	// dialogs can only be used outside "execute()" fn, e.g. in "executePreRun()"
	tMessageBox = (MessageBoxPrototype)GETSYMADDRESS( hndl, "tMessageBox" );
	tYesNoMessageBox = (YesNoMessageBoxPrototype)GETSYMADDRESS( hndl, "tYesNoMessageBox" );
	tGetIntegerInput = (GetIntegerInputPrototype)GETSYMADDRESS( hndl, "tGetIntegerInput" );
	tGetFloatInput = (GetFloatInputPrototype)GETSYMADDRESS( hndl, "tGetFloatInput" );
	tGetTextInput = (GetTextInputPrototype)GETSYMADDRESS( hndl, "tGetTextInput" );
	tGetOpenFileName = (GetOpenFileNamePrototype)GETSYMADDRESS( hndl, "tGetOpenFileName" );
	tGetSaveFileName = (GetSaveFileNamePrototype)GETSYMADDRESS( hndl, "tGetSaveFileName" );
	
	// Basic TBV access functions
	//
	tGetCurrentTimePoint = (GetCurrentTimePointPrototype)GETSYMADDRESS( hndl, "tGetCurrentTimePoint" );
	tGetExpectedNrOfTimePoints = (GetExpectedNrOfTimePointsPrototype)GETSYMADDRESS( hndl, "tGetExpectedNrOfTimePoints" );
	tGetDimsOfFunctionalData = (GetDimsOfFunctionalDataPrototype)GETSYMADDRESS( hndl, "tGetDimsOfFunctionalData" );
	tGetProjectName = (GetProjectNamePrototype)GETSYMADDRESS( hndl, "tGetProjectName" );
	tGetWatchFolder = (GetWatchFolderPrototype)GETSYMADDRESS( hndl, "tGetWatchFolder" );
	tGetTargetFolder = (GetTargetFolderPrototype)GETSYMADDRESS( hndl, "tGetTargetFolder" );
	tGetFeedbackFolder = (GetFeedbackFolderPrototype)GETSYMADDRESS( hndl, "tGetFeedbackFolder" );

	// Protocol/DM/GLM-related functions
	//
	tGetCurrentProtocolCondition = (GetCurrentProtocolConditionPrototype)GETSYMADDRESS( hndl, "tGetCurrentProtocolCondition" );
	tGetFullNrOfPredictors = (GetFullNrOfPredictorsPrototype)GETSYMADDRESS( hndl, "tGetFullNrOfPredictors" );
	tGetCurrentNrOfPredictors = (GetCurrentNrOfPredictorsPrototype)GETSYMADDRESS( hndl, "tGetCurrentNrOfPredictors" );
	tGetNrOfConfoundPredictors = (GetNrOfConfoundPredictorsPrototype)GETSYMADDRESS( hndl, "tGetNrOfConfoundPredictors" );
	tGetValueOfDesignMatrix = (GetValueOfDesignMatrixPrototype)GETSYMADDRESS( hndl, "tGetValueOfDesignMatrix" );
	tGetNrOfContrasts = (GetNrOfContrastsPrototype)GETSYMADDRESS( hndl, "tGetNrOfContrasts" );

	// ROI functions
	//
	tGetNrOfROIs = (GetNrOfROIsPrototype)GETSYMADDRESS( hndl, "tGetNrOfROIs" );
	tGetMeanOfROI = (GetMeanOfROIPrototype)GETSYMADDRESS( hndl, "tGetMeanOfROI" );
	tGetNrOfVoxelsOfROI = (GetNrOfVoxelsOfROIPrototype)GETSYMADDRESS( hndl, "tGetNrOfVoxelsOfROI" );
	tGetBetaOfROI = (GetBetaOfROIPrototype)GETSYMADDRESS( hndl, "tGetBetaOfROI" );
	tGetCoordsOfVoxelOfROI = (GetCoordsOfVoxelOfROIPrototype)GETSYMADDRESS( hndl, "tGetCoordsOfVoxelOfROI" );

	// Full data access functions
	//
	tGetValueOfVoxelAtTime = (GetValueOfVoxelAtTimePrototype)GETSYMADDRESS( hndl, "tGetValueOfVoxelAtTime" );
	tGetTimeCourseData = (GetTimeCourseDataPrototype)GETSYMADDRESS( hndl, "tGetTimeCourseData" );
	tGetBetaOfVoxel = (GetBetaOfVoxelPrototype)GETSYMADDRESS( hndl, "tGetBetaOfVoxel" );
	tGetBetaMaps = (GetBetaMapsPrototype)GETSYMADDRESS( hndl, "tGetBetaMaps" );
	tGetMapValueOfVoxel = (GetMapValueOfVoxelPrototype)GETSYMADDRESS( hndl, "tGetMapValueOfVoxel" );
	tGetContrastMaps = (GetContrastMapsPrototype)GETSYMADDRESS( hndl, "tGetContrastMaps" );
	
	return true;
}

inline bool checkVersionGreaterEqualVersion(int MajorA, int MinorA, int SubMinorA, int MajorB, int MinorB, int SubMinorB)
{
	if(MajorA > MajorB)
		return true;

	if(MajorB > MajorA)
		return false;

	// Major is equal, Minor decides

	if(MinorA > MinorB)
		return true;

	if(MinorB > MinorA)
		return false;

	// Minor is equal, SubMinor decides

	if(SubMinorA >= SubMinorB)
		return true;

	// B smaller
	return false;
}

Q_DECLARE_INTERFACE(TBVPluginInterface, "TBV.TBVPluginInterface/1.0")	// SB: Qt

#endif // TBVPLUGININTERFACE_H
