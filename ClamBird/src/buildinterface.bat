set GECKO="C:\Program Files\gecko-sdk"
set MOZPATH="D:\PROJECTS\mozilla\dist\bin"

%GECKO%\bin\xpidl -m header -I %GECKO%\idl nsIClamwinService.idl
%GECKO%\bin\xpidl -m typelib -I %GECKO%\idl nsIClamwinService.idl

copy nsIClamWinService.xpt %MOZPATH%\components

cd %MOZPATH%
regxpcom

pause