##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=reddit-save-archiver
ConfigurationName      :=Debug
WorkspacePath          :=/media/2TB_Archive/Projects/RSA
ProjectPath            :=/media/2TB_Archive/Projects/RSA/reddit-save-archiver
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Sai
Date                   :=11/08/19
CodeLitePath           :=/home/sai/.codelite
LinkerName             :=/usr/bin/g++
SharedObjectLinkerName :=/usr/bin/g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="reddit-save-archiver.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)curl $(LibrarySwitch)stdc++fs 
ArLibs                 :=  "curl" "stdc++fs" 
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/g++
CC       := /usr/bin/gcc
CXXFLAGS := -pedantic-errors -Wfatal-errors -pedantic -W -Wall -std=c++17 -g -O0 -Wall -std=c++17 $(Preprocessors)
CFLAGS   :=  -g -O0 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/Source.cpp$(ObjectSuffix) $(IntermediateDirectory)/Saver.cpp$(ObjectSuffix) $(IntermediateDirectory)/Reddit.cpp$(ObjectSuffix) $(IntermediateDirectory)/base.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@test -d ./Debug || $(MakeDirCommand) ./Debug


$(IntermediateDirectory)/.d:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/Source.cpp$(ObjectSuffix): Source.cpp $(IntermediateDirectory)/Source.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/media/2TB_Archive/Projects/RSA/reddit-save-archiver/Source.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Source.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Source.cpp$(DependSuffix): Source.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Source.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Source.cpp$(DependSuffix) -MM Source.cpp

$(IntermediateDirectory)/Source.cpp$(PreprocessSuffix): Source.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Source.cpp$(PreprocessSuffix) Source.cpp

$(IntermediateDirectory)/Saver.cpp$(ObjectSuffix): Saver.cpp $(IntermediateDirectory)/Saver.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/media/2TB_Archive/Projects/RSA/reddit-save-archiver/Saver.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Saver.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Saver.cpp$(DependSuffix): Saver.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Saver.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Saver.cpp$(DependSuffix) -MM Saver.cpp

$(IntermediateDirectory)/Saver.cpp$(PreprocessSuffix): Saver.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Saver.cpp$(PreprocessSuffix) Saver.cpp

$(IntermediateDirectory)/Reddit.cpp$(ObjectSuffix): Reddit.cpp $(IntermediateDirectory)/Reddit.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/media/2TB_Archive/Projects/RSA/reddit-save-archiver/Reddit.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Reddit.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Reddit.cpp$(DependSuffix): Reddit.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Reddit.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Reddit.cpp$(DependSuffix) -MM Reddit.cpp

$(IntermediateDirectory)/Reddit.cpp$(PreprocessSuffix): Reddit.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Reddit.cpp$(PreprocessSuffix) Reddit.cpp

$(IntermediateDirectory)/base.cpp$(ObjectSuffix): base.cpp $(IntermediateDirectory)/base.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/media/2TB_Archive/Projects/RSA/reddit-save-archiver/base.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/base.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/base.cpp$(DependSuffix): base.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/base.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/base.cpp$(DependSuffix) -MM base.cpp

$(IntermediateDirectory)/base.cpp$(PreprocessSuffix): base.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/base.cpp$(PreprocessSuffix) base.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Debug/


