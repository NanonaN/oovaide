/*
* Project.h
*
*  Created on: Jun 19, 2013
*  \copyright 2013 DCBlaha.  Distributed under the GPL.
*/

#ifndef PROJECT_H_
#define PROJECT_H_

#include <string>
#include "FilePath.h"
#include "NameValueFile.h"
#include "Packages.h"
#include "OovString.h"

#define OptSourceRootDir "SourceRootDir"
#define OptProjectExcludeDirs "ExcludeDirs"
#define OptBaseArgs "BuildArgsBase"

// These are stored per configuration. They will have the build configuration
// name appended.
#define OptExtraBuildArgs "BuildArgsExtra"

// This is for user custom build configurations.
#define OptBuildConfigs "BuildConfigs"

// These are stored per configuration. They will have the build configuration
// name appended.
#define OptToolLibPath "Tool-LibPath"
#define OptToolCompilePath "Tool-CompilerPath"
// During analysis, the compiler is typically "java", but also requires JavaAnalyzerTool.
// During debug or release or other, the compiler is typically "javac".
#define OptToolJavaCompilePath "Tool-JavaCompilerPath"
#define OptToolJavaAnalyzerTool "Tool-JavaAnalyzerTool"
#define OptToolJavaJarToolPath "Tool-JavaJarToolPath"           // Actually the jar name.
#define OptToolObjSymbolPath "Tool-ObjSymbolPath"

#define OptToolDebuggerPath "Tool-DebuggerPath"

#define OptJavaClassPath "Java-ClassPath"
#define OptJavaJdkPath "Java-JdkPath"
#define OptJavaAnalyzeArgs "Java-Args-Analyze"
#define OptJavaBuildArgs "Java-Args-Build"

#define BuildConfigAnalysis "Analyze"
#define BuildConfigDebug "Debug"
#define BuildConfigRelease "Release"

#define DupsDir "dups"
#define DupsHashExtension "hsh"

OovString makeBuildConfigArgName(OovStringRef const baseName,
        OovStringRef const buildConfig);

enum eProcessModes
    {
    PM_None, PM_Analyze, PM_Build, PM_CovInstr, PM_CovBuild, PM_CovStats,

    PM_CleanMask=0xF00,
    PM_CleanAnalyze=0x100, PM_CleanBuild=0x200, PM_CleanCoverage=0x400
    };

/// A project file contains compile flags and drawing parameters.
/// There is also a matching .XMI file for every source file, that only
/// contains content that is needed to display diagrams.
class Project
    {
    public:
        static void setArgv0(OovStringRef arg);
        static void setProjectDirectory(OovStringRef const projDir)
            { sProjectDirectory = projDir; }
        static OovString const &getProjectDirectory()
            { return sProjectDirectory; }
        static OovString const &getBinDirectory();
        static OovString const &getLibDirectory();
        static OovString getProjectFilePath();

        static OovString getGuiOptionsFilePath();

        static void setSourceRootDirectory(OovStringRef const projDir)
            { sSourceRootDirectory = projDir; }
        static OovString const &getSourceRootDirectory()
            { return sSourceRootDirectory; }

        static OovString getComponentTypesFilePath();
        static OovString getComponentSourceListFilePath();

        static char const *getRootComponentName()
            { return "<Root>"; }
        // This is the name without a path.
        static OovString getRootComponentFileName()
            {
            FilePath path(sSourceRootDirectory, FP_Dir);
            return path.getPathSegment(path.getPosLeftPathSep(
                    path.getPosEndDir(), RP_RetPosNatural)+1);
            }

        static OovString getPackagesFilePath();
        static OovString getBuildPackagesFilePath();

        /// buildDirClass = BuildConfigAnalysis, BuildConfigDebug, etc.
        static FilePath getBuildOutputDir(OovStringRef const buildDirClass);
        static FilePath getIntermediateDir(OovStringRef const buildDirClass);

        static FilePath getOutputDir();

        static OovStringRef const getSrcRootDirectory();
        /// Returns a filename relative to the root source directory.
        /// Returns the full file name if it is not relative.
        static OovString getSrcRootDirRelativeSrcFileName(OovStringRef const srcFileName,
                OovStringRef const srcRootDir);
        static OovString getSrcRootDirRelativeSrcFileName(OovStringRef const srcFileName);
        static OovString getSrcRootDirRelativeSrcFileDir(OovStringRef const srcRootDir,
                OovStringRef const srcFileName);
        static OovString getSrcRootDirRelativeSrcFileDir(OovStringRef const absSrcFileDir);
        static OovString getAbsoluteDirFromSrcRootDirRelativeDir(OovStringRef const relSrcFileDir);

        static OovStringRef getAnalysisIncDepsFilename()
            { return "oovaide-incdeps.txt"; }
        static OovStringRef getAnalysisComponentsFilename()
            { return "oovaide-extdirs.txt"; }
        /// Make a filename for the compressed content file for each source file.
        /// The analysisDir is retreived from the build configuration.
        static OovString makeAnalysisFileName(OovStringRef const srcFileName,
                OovStringRef const srcRootDir, OovStringRef const analysisDir);

        // Location for coverage source files
        static OovString getCoverageSourceDirectory();
        static OovString makeCoverageSourceFileName(OovStringRef const srcFileName,
                OovStringRef const srcRootDir);
        static OovString getCoverageProjectDirectory();
        static OovStringRef const getCovLibName()
            { return "covLib"; }

        // This does not have an extension.
        static OovString makeOutBaseFileName(OovStringRef const srcFileName,
                OovStringRef const srcRootDir, OovStringRef const outFilePath);
        // This does not have an extension.
        // This converts the filename so that it does not nest into subdirectories.
        static OovString makeTreeOutBaseFileName(OovStringRef const srcFileName,
                OovStringRef const srcRootDir, OovStringRef const outFilePath);
        // This recovers the actual source file name that was changed by
        // makeTreeOutBaseFileName.
        static OovString recoverFileName(OovStringRef const srcFileName);
    private:
        static OovString sProjectDirectory;
        static OovString sSourceRootDirectory;
#ifndef __linux__
        static OovString sArgv0;
#endif
    };

enum eLinkOrderIndices { LOI_InternalProject=0, LOI_AfterInternalProject=1000,
    LOI_PackageIncrement=1000 };
struct IndexedString
    {
    size_t mLinkOrderIndex;
    std::string mString;
    IndexedString(size_t index, OovStringRef const str):
        mLinkOrderIndex(index), mString(str)
        {}
    IndexedString(size_t index, const std::string &str):
        mLinkOrderIndex(index), mString(str)
        {}
    bool operator<(const IndexedString &rhs) const
        { return(mLinkOrderIndex < rhs.mLinkOrderIndex); }
    };

typedef std::vector<IndexedString> IndexedStringVec;
typedef std::set<IndexedString> IndexedStringSet;

class ProjectReader:public NameValueFile
    {
    public:
        ProjectReader()
            {}
        OovStatusReturn readProject(OovStringRef const oovProjectDir);
        static OovStringRef const getSrcRootDirectory()
            { return Project::getSrcRootDirectory(); }
    };

class ProjectBuildArgs
    {
    public:
        ProjectBuildArgs(ProjectReader &project):
            mProjectOptions(project), mProjectPackages(false), mBuildPackages(false),
            mVerbose(false)
            {}
        void loadBuildArgs(OovStringRef const buildConfigName);

        const ProjectPackages &getProjectPackages() const
            { return mProjectPackages; }
        BuildPackages const &getBuildPackages() const
            { return mBuildPackages; }
        BuildPackages &getBuildPackages()
            { return mBuildPackages; }
        const OovStringVec &getCompileArgs() const
            { return mCompileArgs; }
        const IndexedStringVec &getLinkArgs() const
            { return mLinkArgs; }
        const OovStringVec &getExternalArgs() const
            { return mExternalRootArgs; }
        unsigned int getExternalPackageLinkOrder(OovStringRef const pkgName) const;
        // These are only used for computing CRC's. The builder will later get
        // the arguments from the packages because not all packages are used by
        // all components.
        const OovStringVec getAllCrcCompileArgs() const;
        const OovStringVec getAllCrcLinkArgs() const;
        /// These are absolute directories
        CompoundValue getProjectExcludeDirs() const;
        bool getVerbose() const
            { return mVerbose; }

    private:
        ProjectReader &mProjectOptions;
        OovStringVec mCompileArgs;
        IndexedStringVec mLinkArgs;
        OovStringVec mExternalRootArgs;
        IndexedStringVec mExternalPackageNames;

        // The Crc vectors are only for computing CRC and are not used for building.
        OovStringVec mPackageCrcCompileArgs;
        OovStringVec mPackageCrcLinkArgs;
        OovStringVec mPackageCrcNames;

        /// These are chosen by the user and saved for the project.
        ProjectPackages mProjectPackages;

        /// These may be modified by the build and contains project packages and
        /// external root directories.
        BuildPackages mBuildPackages;
        bool mVerbose;

        void addCompileArg(OovStringRef const str)
            { mCompileArgs.push_back(str); }
        void addLinkArg(unsigned int linkOrderIndex, OovStringRef const str)
            { mLinkArgs.push_back(IndexedString(linkOrderIndex, str)); }
        void addExternalArg(OovStringRef const str)
            { mExternalRootArgs.push_back(str); }
        void addExternalPackageName(unsigned int linkOrderIndex, OovStringRef const str)
            { mExternalPackageNames.push_back(IndexedString(linkOrderIndex, str)); }

        void addPackageCrcName(OovStringRef const pkgName)
            { mPackageCrcNames.push_back(OovString("-EP") + OovString(pkgName)); }
        void addPackageCrcCompileArg(OovStringRef const str)
            { mPackageCrcCompileArgs.push_back(str); }
        void addPackageCrcLinkArg(OovStringRef const str)
            { mPackageCrcLinkArgs.push_back(str); }

        void parseArgs(OovStringVec const &strs);
        void handleExternalPackage(OovStringRef const pkgName);
    };


#endif /* PROJECT_H_ */
