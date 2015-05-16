-- This LUA script losely mimics the 'make distcheck' used on Linux
-- to build a source code package. In many ways it's quite different,
-- though. It expects all tools as ready-made executables and instead
-- builds Nut/OS binaries for all supported targets.
--
-- The result will be an executable Windows installation file. Any
-- build errors are logged in the ERROR_LOG.
--
-- The script works on my machine and requires a number of external
-- software, like NSIS, Doxygen, AVR and ARM compilers etc. It may
-- not work for you.
--
-- Written by Harald Kipp
--
-- I'm a lousy LUA programmer and will be happy to receive your
-- suggestions and comments.
--
-- Released to the Public Domain
--

start_time = os.time()
total_time = 0

require("lfs")

PKG_VERSION = "4.10.1"
ARC_NAME = "ethernut-"..PKG_VERSION
TOP_DIR = "e:\\ethernut"
PS = "\\"
SRC_DIR = TOP_DIR..PS.."nut-4_10"
WORK_DIR = TOP_DIR..PS..ARC_NAME
REDIR = " >>build4a.log 2>&1"
ERROR_LOG = "errors-"..PKG_VERSION..".log"
STATS_LOG = "stats-"..PKG_VERSION..".log"

BASE_PATH = "E:\\apps\\bin;%SystemRoot%\\system32;%SystemRoot%;%SystemRoot%\\System32\\Wbem;"
TOOL_PATH = "E:\\ethernut\\nut-4_10_rc1\\nut\\tools\\win32;"
GCCAVR_PATH = "E:\\WinAVR\\bin;E:\\WinAVR\\utils\\bin;"
GCCAVR32_PATH = "%ProgramFiles(x86)%\\Atmel\\AVR Tools\\AVR32 Toolchain\\bin;"
GCCARM_PATH = "%ProgramFiles(x86)%\\yagarto\\bin;"
ICCAVR7_PATH = "E:\\iccv719avr\\bin;"
NSIS_PATH = "%ProgramFiles(x86)%\\NSIS;"
DOXY_PATH = "%ProgramFiles(x86)%\\doxygen\\bin;%ProgramFiles(x86)%\\Graphviz2.27\\bin;%ProgramFiles(x86)%\\HTML Help Workshop;"

avr_targets = {
  { name = "arthernet1", mcu = "atmega128" },
  { name = "charon2", mcu = "atmega128" },
  { name = "ethernut103", mcu = "atmega103" },
  { name = "ethernut13f", mcu = "atmega128" },
  { name = "ethernut13g", mcu = "atmega128" },
  { name = "ethernut13h", mcu = "atmega128" },
  { name = "ethernut13h-256gcc", mcu = "atmega2561" },
  { name = "ethernut20a", mcu = "atmega128" },
  { name = "ethernut21b", mcu = "atmega128" },
  { name = "ethernut21b-256gcc", mcu = "atmega2561" },
  { name = "mmnet01", mcu = "atmega128" },
  { name = "mmnet02_03_04", mcu = "atmega128" },
  { name = "mmnet101", mcu = "atmega128" },
  { name = "mmnet102_103_104", mcu = "atmega128" },
  { name = "stk501", mcu = "atmega128" },
  { name = "xnut-100", mcu = "atmega128" },
  { name = "xnut-105c", mcu = "at90can128" },
  { name = "xnut-105d", mcu = "at90can128" },
}

avr32_targets = {
  { name = "evk1100-extram", mcu = "uc3a0512es" },
  { name = "evk1100-intram", mcu = "uc3a0512es" },
  { name = "evk1101", mcu = "uc3b0256" },
  { name = "evk1104-extram", mcu = "uc3a3256" },
  { name = "evk1104-intram", mcu = "uc3a3256" },
  { name = "evk1105-extram", mcu = "uc3a0512" },
  { name = "evk1105-intram", mcu = "uc3a0512" }
}

arm_targets = {
  { name = "at91eb40a", mcu = "arm7tdmi" },
  { name = "at91sam7s", mcu = "arm7tdmi" },
  { name = "at91sam7se-ek", mcu = "arm7tdmi" },
  { name = "at91sam7x-ek", mcu = "arm7tdmi" },
  { name = "at91sam9260-ek", mcu = "arm9" },
  { name = "at91sam9g45-ek", mcu = "arm9" },
  { name = "eir10c", mcu = "arm7tdmi" },
  { name = "enet-sam7x", mcu="arm7tdmi" },
  { name = "ethernut30d", mcu = "arm7tdmi" },
  { name = "ethernut30e", mcu = "arm7tdmi" },
  { name = "ethernut31d", mcu = "arm7tdmi" },
  { name = "ethernut50f", mcu = "arm9" },
  { name = "gbaxport2", mcu = "arm7tdmi" },
  { name = "morphoq11a", mcu = "arm7tdmi" },
  { name = "olimex-sam7-ex256", mcu = "arm7tdmi" },
}

asm_sources = {
  { p = ".S$", m = ".S", r = true },
  { p = ".s$", m = ".s", r = true },
}

app_specials = {
  { p = ".asp$", m = ".asp", r = true },
  { p = ".class$", m = ".class", r = true },
  { p = ".gif$", m = ".gif", r = true },
  { p = ".html$", m = ".html", r = true },
  { p = ".isp$", m = ".isp", r = true },
  { p = ".inc$", m = ".inc", r = true },
  { p = ".java$", m = ".java", r = true },
  { p = ".mp3$", m = ".mp3", r = true },
  { p = ".shtml$", m = ".shtml", r = true },
  { p = ".swf$", m = ".swf", r = true },
  { p = ".xml$", m = ".xml", r = true },
}

c_sources = {
  { p = ".c$", m = ".c", r = true },
  { p = ".h$", m = ".h", r = true },
  { p = ".txt$", m = ".txt", r = true },
}

boot_specials = {
  { p = ".jom$", m = ".jom", r = true },
}

cpp_sources = {
  { p = ".cc$", m = ".cc", r = true },
  { p = ".cpp$", m = ".cpp", r = true },
  { p = ".h$", m = ".h", r = true },
  { p = ".hpp$", m = ".hpp", r = true },
  { p = ".pro$", m = ".pro", r = true },
  { p = ".qrc$", m = ".qrc", r = true },
  { p = ".rc$", m = ".rc", r = true },
  { p = ".ui$", m = ".ui", r = true },
  { p = ".xpm$", m = ".xpm", r = true },
  { p = ".bmp$", m = ".bmp", r = true },
  { p = ".ico$", m = ".ico", r = true },
  { p = ".png$", m = ".png", r = true },
}

doxy_files = {
  { p = ".css$", m = ".css", r = true },
  { p = ".cfg$", m = ".cfg", r = true },
  { p = ".gif$", m = ".gif", r = true },
  { p = ".html$", m = ".html", r = true },
  { p = ".in$", m = ".in", r = true },
  { p = ".txt$", m = ".txt", r = true },
}

make_files = {
  { p = "^Makefile.am", m = "Makefile.am", r = false },
  { p = "^Makefile", m = "Makefile", r = true },
  { p = "^Makedefs", m = "Makedefs", r = true },
  { p = "^Makevars", m = "Makevars", r = true },
  { p = "^Makerules", m = "Makerules", r = true },
  { p = "^Makeburn", m = "Makeburn", r = true },
}

nsis_files = {
  { p = ".bmp$", m = ".bmp", r = true },
  { p = ".nsi$", m = ".nsi", r = true },
  { p = ".txt$", m = ".txt", r = true },
  { p = ".ico$", m = ".ico", r = true },
  { p = "^ChangeLog", m = "ChangeLog", r = true },
}

icc_projects = {
  { p = ".prj$", m = ".prj", r = true },
  { p = ".SRC$", m = ".SRC", r = true },
}

top_files = {
  { p = "^ChangeLog", m = "ChangeLog", r = true },
  { p = "^AUTHORS", m = "AUTHORS", r = true },
  { p = "^COPYING", m = "COPYING", r = true },
  { p = "^INSTALL", m = "INSTALL", r = true },
  { p = "^NEWS", m = "NEWS", r = true },
  { p = "^README", m = "README", r = true },
  { p = "^nutsetup", m = "nutsetup", r = true },
}

win32_files = {
  { p = ".exe$", m = ".exe", r = true },
  { p = ".dll$", m = ".dll", r = true },
  { p = ".cfg$", m = ".cfg", r = true },
  { p = ".conf$", m = ".conf", r = true },
  { p = ".ini$", m = ".ini", r = true },
  { p = ".bat$", m = ".bat", r = true },
  { p = ".sys$", m = ".sys", r = true },
  { p = ".jom$", m = ".jom", r = true },
  { p = ".tcl$", m = ".tcl", r = true },
  { p = ".bin$", m = ".bin", r = true },
  { p = ".manifest$", m = ".manifest", r = true },
}

tool_files = {
  { p = "^Makefile", m = "Makefile", r = true },
  { p = ".script$", m = ".script", r = true },
}

lua_files = {
  { p = "^COPYRIGHT", m = "COPYRIGHT", r = true },
  { p = "^HISTORY", m = "HISTORY", r = true },
  { p = "^INSTALL", m = "INSTALL", r = true },
  { p = ".pri$", m = ".pri", r = true },
  { p = ".pc$", m = ".pc", r = true },
  { p = ".bat$", m = ".bat", r = true },
  { p = ".lua$", m = ".lua", r = true },
}

linker_scripts = {
  { p = ".ld$", m = ".ld", r = true },
}

config_files = {
  { p = ".conf$", m = ".conf", r = true },
  { p = ".nut$", m = ".nut", r = true },
}

text_files = {
  { p = ".txt$", m = ".txt", r = true },
}


function check_filter(name, filter)
  local basename = string.gsub(name, ".+/", "")

  for i,v in ipairs(filter) do 
    for idx,val in ipairs(v) do
      -- print("Check "..basename.." against "..val.m)	
      if basename:match(val.p) == val.m then
        return val.r
      end
    end
  end
  return false
end

function copy_files(source, dest, filter)
   local attr = lfs.attributes(source);

   if attr == nil then
     print("Missing "..source)
     error(e)
   elseif attr.mode == "directory" then
      lfs.mkdir(dest)
      for child in lfs.dir(source) do
        if child ~= "." and child ~= ".." then
          copy_files(source.."/"..child, dest.."/"..child, filter)
        end
      end
  elseif check_filter(source, filter) then
      local f = io.open(source, "rb")
      local data = f:read("*all*")
      f:close()
      f,e = io.open(dest, "wb")
      if e then
         print("Failed to open "..dest)
         error(e)
      end
      f:write(data)
      f:close()
      lfs.touch(dest, attr.access, attr.modification)
   end
end

function update_config(filename, key, value)
  local replaced = false
  local line
  local k
  
  local f = io.open(filename)
  local data = f:read("*all*")
  f:close()
  f,e = io.open(filename..".bak", "w")
  if e then
     print(filename..".bak")
     error(e)
  end
  f:write(data)
  f:close()
  
  f = io.open(filename, "w")
  
  for line in io.lines(filename..".bak") do
    if string.len(line) > 1 and string.sub(line, 1, 2) ~= "--" then
      k = string.match(line, "([%w_]+)");
      if replaced == false and k == key then
        if value == nil then
          line = "-- "..line
        else
          line = key.." = "..value
        end
        replaced = true
      end
    end
    f:write(line.."\n")
  end
  if replaced == false then
    f:write(key.." = "..value.."\n")
  end
  io.close(f)
end

function append_log_tail(f, log, num)
  local count = 0
  for line in io.lines(log) do 
    count = count + 1
  end
  local skip = 0
  if count > num then
    skip = count - num
  end
  for line in io.lines(log) do 
    if skip > 0 then
      skip = skip - 1
    else
      f:write(line)
      f:write("\n")
    end
  end
end

function log_build_error(target, cmd, result)
  build_result:write(string.rep("-", 72).."\n")
  build_result:write("Target : "..target.."\n")
  build_result:write("Command: "..cmd.."\n")
  build_result:write("Error  : "..result.."\n")
  build_result:write(string.rep("-", 72).."\n")
  append_log_tail(build_result, "build4a.log", 10)
  build_result:write("\n\n")
  build_result:flush()
  build_stats:write(cmd.." interrupted due to an error, check "..ERROR_LOG.."\n")
  build_stats:flush()
end

function create_readme(path, content)
  local f
  local e
  f,e = io.open(path..PS.."README", "w")
  if e then
    print("Failed to create "..path..PS.."README")
  else
    f:write(content)
    f:close()
  end
  return e
end

function create_target_readme(path, board, platform)
  local content
  content = "Board   : "..board.."\n"
  content = content.."Compiler: "..platform.."\n"
  return create_readme(path, content)
end

function build_libs(board, platform, path)
  local run_time = os.time()

  print(board..": Building "..platform.." libraries")
  
  local target = board.."-"..platform.."-bld"
  local cmd = "..\\nutconfigure.exe -b"..target.." -cnut/conf/"..board..".conf -l"..target.."/lib -m"..platform.." create-buildtree"
  local result = os.execute(cmd..REDIR)
  if result ~= 0 then
    log_build_error(target, cmd, result)
  else
    lfs.chdir(target)
    cmd = "SET PATH="..path.."&make clean all install"
    result = os.execute(cmd..REDIR)
    if result ~= 0 then
      log_build_error(target, cmd, result)
      lfs.chdir("..")
    else
      lfs.chdir("..")
      cmd = "xcopy "..target..PS.."lib nut"..PS.."lib"..PS.."boards"..PS..board..PS..platform.."\\ /S /R /Y"
      if os.execute(cmd..REDIR) ~= 0 then
        log_build_error(target, cmd, result)
      end
      create_target_readme("nut"..PS.."lib"..PS.."boards"..PS..board..PS..platform, board, platform)
    end
  end

  run_time = os.time() - run_time
  total_time = total_time + run_time
  build_stats:write(board..": Building "..platform.." libraries took "..run_time.." seconds\n")
  build_stats:flush()
  return result
end

function build_apps(board, mcu, platform, path)
  local run_time = os.time()

  print(board..": Building "..platform.." samples")
  local target = board.."-"..platform
  local cmd = "..\\nutconfigure.exe -a"..target.."-app -b"..target.."-bld -cnut/conf/"..board..".conf -l"..target.."-bld/lib -m"..platform.." create-apptree"
  local result = os.execute(cmd..REDIR)
  if result ~= 0 then
    log_build_error(target.."-app", cmd, result)
  else
    os.execute("mkdir nut"..PS.."bin"..PS..mcu..REDIR)
    create_readme("nut"..PS.."bin"..PS..mcu, "When running 'make install' binaries for "..mcu.." will be placed in this directory.\n")
    os.execute("rmdir /S /Q nut"..PS.."bin"..PS..board..REDIR)

    lfs.chdir(target.."-app")
    cmd = "SET PATH="..path.."&make clean all install"
    result = os.execute(cmd..REDIR)
    if result ~= 0 then
      log_build_error(target.."-app", cmd, result)
      lfs.chdir("..")
    else
      lfs.chdir("..")
      cmd = "xcopy nut"..PS.."bin"..PS..mcu.."\\*.hex nut"..PS.."bin"..PS.."boards"..PS..board..PS..platform.."\\ /S /R /Y"
      os.execute(cmd..REDIR)
      cmd = "del nut"..PS.."bin"..PS..mcu.."\\*.hex /F /Q"
      os.execute(cmd..REDIR)
      cmd = "xcopy nut"..PS.."bin"..PS..mcu.."\\*.bin nut"..PS.."bin"..PS.."boards"..PS..board..PS..platform.."\\ /S /R /Y"
      os.execute(cmd..REDIR)
      cmd = "del nut"..PS.."bin"..PS..mcu.."\\*.bin /F /Q"
      os.execute(cmd..REDIR)
      create_target_readme("nut"..PS.."bin"..PS.."boards"..PS..board..PS..platform, board, platform)
    end
  end

  run_time = os.time() - run_time
  total_time = total_time + run_time
  build_stats:write(board..": Building "..platform.." samples took "..run_time.." seconds\n")
  build_stats:flush()
  return result
end

function build_docs(path)
  local f
  local e
  local line
  local run_time = os.time()

  print("Building dox")

  f,e = io.open("chm_en.cfg", "w")
  if e then
     print("chm_en.cfg")
     error(e)
  end

  for line in io.lines("chm_en.cfg.in") do 
    line = string.gsub(line, "@PACKAGE_VERSION@", PKG_VERSION)
    line = string.gsub(line, "@top_srcdir@", "/ethernut/nut-4_10")
    line = string.gsub(line, "@top_builddir@", "/ethernut/ethernut-"..PKG_VERSION)
    line = string.gsub(line, "@srcdir@", ".")
    f:write(line.."\n")
  end
  f:close()

  f,e = io.open("nut_en.cfg", "w")
  if e then
     print("nut_en.cfg")
     error(e)
  end

  for line in io.lines("nut_en.cfg.in") do 
    line = string.gsub(line, "@PACKAGE_VERSION@", PKG_VERSION)
    line = string.gsub(line, "@top_srcdir@", "/ethernut/nut-4_10")
    line = string.gsub(line, "@top_builddir@", "/ethernut/ethernut-"..PKG_VERSION)
    line = string.gsub(line, "@srcdir@", ".")
    f:write(line.."\n")
  end
  f:close()
  
  local cmd = "SET PATH="..path.."&doxygen chm_en.cfg&doxygen nut_en.cfg"
  local result = os.execute(cmd..REDIR)
  if result ~= 0 then
    log_build_error("docs", cmd, result)
  end
  run_time = os.time() - run_time
  total_time = total_time + run_time
  build_stats:write("Building dox took "..run_time.." seconds\n")
  build_stats:flush()
end

function build_winhelp(path)
  local run_time = os.time()
  print("Building Windows help file")

  local cmd = "SET PATH="..path.."&hhc index.hhp"
  local result = os.execute(cmd..REDIR)
  if result ~= 0 then
    log_build_error("winhelp", cmd, result)
  end
  run_time = os.time() - run_time
  total_time = total_time + run_time
  build_stats:write("Building Windows help file "..run_time.." seconds\n")
  build_stats:flush()
end

function build_winstaller(path)
  local run_time = os.time()
  print("Building Windows installer")

  local cmd = "SET PATH="..path.."&makensis ethernut.nsi"
  local result = os.execute(cmd..REDIR)
  if result ~= 0 then
    log_build_error("wininstaller", cmd, result)
  end
  run_time = os.time() - run_time
  total_time = total_time + run_time
  build_stats:write("Building Windows installer took "..run_time.." seconds\n")
  build_stats:flush()
end

build_result = io.open(ERROR_LOG, "w")
build_stats = io.open(STATS_LOG, "w")

--
-- Copy all files from source to work directory
--
local run_time = os.time()

print("Creating work directory")
copy_files(SRC_DIR, WORK_DIR, { asm_sources, c_sources, cpp_sources, make_files, linker_scripts, top_files } )
copy_files(SRC_DIR..PS.."nut"..PS.."boot", WORK_DIR..PS.."nut"..PS.."boot", { boot_specials } )
copy_files(SRC_DIR..PS.."nut"..PS.."conf", WORK_DIR..PS.."nut"..PS.."conf", { config_files } )
copy_files(SRC_DIR..PS.."nut"..PS.."app", WORK_DIR..PS.."nut"..PS.."app", { app_specials } )
copy_files(SRC_DIR..PS.."nut"..PS.."hwtest", WORK_DIR..PS.."nut"..PS.."hwtest", { app_specials } )
copy_files(SRC_DIR..PS.."nut"..PS.."appicc", WORK_DIR..PS.."nut"..PS.."appicc", { icc_projects } )
copy_files(SRC_DIR..PS.."nut"..PS.."doc", WORK_DIR..PS.."nut"..PS.."doc", { doxy_files } )
copy_files(SRC_DIR..PS.."nut"..PS.."tools"..PS.."packaging"..PS.."nsis", WORK_DIR..PS.."nut"..PS.."tools"..PS.."packaging"..PS.."nsis", { nsis_files } )
copy_files(SRC_DIR..PS.."nut"..PS.."tools", WORK_DIR..PS.."nut"..PS.."tools", { tool_files } )
copy_files(SRC_DIR..PS.."nut"..PS.."tools"..PS.."qnutconf"..PS.."src"..PS.."lua", WORK_DIR..PS.."nut"..PS.."tools"..PS.."qnutconf"..PS.."src"..PS.."lua", { lua_files } )
copy_files(SRC_DIR..PS.."nut"..PS.."tools"..PS.."win32", WORK_DIR..PS.."nut"..PS.."tools"..PS.."win32", { win32_files } )
run_time = os.time() - run_time
total_time = total_time + run_time
build_stats:write("Creating work directory took "..run_time.." seconds\n")
build_stats:flush()

lfs.chdir(WORK_DIR)

--
-- Build AVR GCC libs and apps
--
path_env = TOOL_PATH..GCCAVR_PATH..BASE_PATH
if os.execute("SET PATH="..path_env.."&avr-gcc -v"..REDIR) == 0 then
  for t_index, t_board in ipairs(avr_targets) do 
    if build_libs(t_board.name, "avr-gcc", path_env) == 0 then
      if build_apps(t_board.name, t_board.mcu, "avr-gcc", path_env) == 0 then
        if build_libs(t_board.name, "avr-gccdbg", path_env) == 0 then
          -- Exclude debug apps from targets with less than 256k flash
          if t_board.mcu == "atmega2561" then
            build_apps(t_board.name, t_board.mcu, "avr-gccdbg", path_env)
          end
        end
      end
    end
  end
else
  build_result:write("GCC for AVR not available\n\n")
  print("AVR GCC build skipped")
end

--
-- Build ARM GCC libs and apps
--
path_env = TOOL_PATH..GCCARM_PATH..BASE_PATH
if os.execute("SET PATH="..path_env.."&arm-none-eabi-gcc -v"..REDIR) == 0 then
  for t_index, t_board in ipairs(arm_targets) do 
    update_config("nut/conf/"..t_board.name..".conf", "CRT_UNSETENV_POSIX", '""')
	update_config("nut/conf/"..t_board.name..".conf", "ARMEABI", '""')
  
    if build_libs(t_board.name, "arm-gcc", path_env) == 0 then
      build_apps(t_board.name, t_board.mcu, "arm-gcc", path_env)
    end
    if build_libs(t_board.name, "arm-gccdbg", path_env) == 0 then
      -- Exclude debug apps from AT91EB40A because of lack of sufficient RAM
      if t_board.name ~= "at91eb40a" then
        build_apps(t_board.name, t_board.mcu, "arm-gccdbg", path_env)
      end
    end
  end
else  
  build_result:write("GCC for ARM not available\n\n")
  print("ARM GCC build skipped")
end

--
-- Build AVR32 GCC libs and apps
--
path_env = TOOL_PATH..GCCAVR32_PATH..BASE_PATH
if os.execute("SET PATH="..path_env.."&avr32-gcc -v"..REDIR) == 0 then
  for t_index, t_board in ipairs(avr32_targets) do 
    if build_libs(t_board.name, "avr32-gcc", path_env) == 0 then
      build_apps(t_board.name, t_board.mcu, "avr32-gcc", path_env)
    end
    if build_libs(t_board.name, "avr32-gccdbg", path_env) == 0 then
      build_apps(t_board.name, t_board.mcu, "avr32-gccdbg", path_env)
    end
  end
else
  build_result:write("SET PATH="..path_env.."&avr32-gcc -v\n")
  build_result:write("GCC for AVR32 not available\n\n")
end

--
-- Build ICCAVR7 libs and apps
--
path_env = TOOL_PATH..ICCAVR7_PATH..BASE_PATH
if os.execute("SET PATH="..path_env.."&iccavr -v"..REDIR) == 0 then
  for t_index, t_board in ipairs(avr_targets) do 
    update_config("nut/conf/"..t_board.name..".conf", "AVR_GCC", nil)
    update_config("nut/conf/"..t_board.name..".conf", "ICCAVR", '""')
    update_config("nut/conf/"..t_board.name..".conf", "ICCAVR_STARTUP", '"crtenutram"')
    if t_board.mcu == "atmega128" then
      if build_libs(t_board.name, "avr-icc7", path_env) == 0 then
        build_apps(t_board.name, "enhanced", "avr-icc7", path_env)
      end
    elseif t_board.mcu == "atmega2561" then
      if build_libs(t_board.name, "avrext-icc7", path_env) == 0 then
        build_apps(t_board.name, "extended", "avrext-icc7", path_env)
      end
    end
    update_config("nut/conf/"..t_board.name..".conf", "AVR_GCC", '""')
    update_config("nut/conf/"..t_board.name..".conf", "ICCAVR", nil)
    update_config("nut/conf/"..t_board.name..".conf", "ICCAVR_STARTUP", nil)
  end
else
  build_result:write("ICC7 for AVR not available\n\n")
  print("ICC AVR build skipped")
end

--
-- Generate API documentation
--
path_env = DOXY_PATH..BASE_PATH
lfs.chdir("nut\\doc\\gen")
build_docs(path_env)
lfs.chdir("..\\..\\..")

lfs.chdir("nut\\doc\\en\\chm")
build_winhelp(path_env)
lfs.chdir("..\\..\\..\\..")

--
-- Generate Windows installer executable
--
path_env = NSIS_PATH..BASE_PATH
lfs.chdir("nut\\tools\\packaging\\nsis")
build_winstaller(path_env)
lfs.chdir("..\\..\\..\\..")

lfs.chdir("..")

run_time = os.time() - start_time;
build_stats:write("Overhead was "..(run_time - total_time).." seconds, thank you for your patience.\n")
build_stats:write("Total time was "..run_time.." seconds, thank you for your patience.\n")

io.close(build_result)
io.close(build_stats)
