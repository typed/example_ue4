rem 1.下载安装msys2
rem http://www.msys2.org/下载msys2
rem 下载安装完成后，在msys2的shell中安装编译FFMPEG必要的命令行工具
rem pacman -S make gcc diffutils yasm pkg-config
rem 2.配置编译环境
rem 编辑C:\msys64\msys2_shell.cmd
rem 将
rem rem set MSYS2_PATH_TYPE=inherit 
rem 修改为
rem set MSYS2_PATH_TYPE=inherit
rem 到开始菜单，执行“Visual Studio 2015-> VS2015 x64 本机工具命令提示符”,在命令窗口下执行
rem C:\msys64\msys2_shell.cmd
rem 重命名C:\msys64\usr\bin\link.exe 为C: \msys64\usr\bin\link.bak, 避免和MSVC 的link.exe抵触。
rem 查看cl和link工具是不是VS2015中的
rem 3.编译FFMPEG
rem 在上一步打开的msys2的shell中进入到ffmpeg源码目录，编译ffmpeg
PREFIX=$(pwd)/build_result/
./configure \
    --toolchain=msvc \
    --prefix=${PREFIX} \
    --disable-doc \
    --disable-programs
    
make clean
make -j4
make install