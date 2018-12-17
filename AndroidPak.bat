echo %cd%
set UE4Path=D:\Program Files\Epic Games\UE_4.18\
set ProjectPath=%cd%\example_ue4.uproject
set PkgPath=%cd%\Build
"%UE4Path%Engine\Build\BatchFiles\RunUAT.bat" BuildCookRun -nop4 -project=%ProjectPath% -cook -cookflavor=ETC2 -build -stage -pak -archive -archivedirectory=%PkgPath% -package -holdCookContent=true -strip -targetplatform=Android