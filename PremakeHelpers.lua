local symbol_table = {}

-- Default Output Format
symbol_table.OutputDirectory = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}";

-- C File Includes
symbol_table.IncludeCHeaders = "%{prj.name}/src/**.h";
symbol_table.IncludeCSources = "%{prj.name}/src/**.c";

-- C++ File Includes
symbol_table.IncludeCPPHeaders = "%{prj.name}/src/**.hpp";
symbol_table.IncludeCPPSources = "%{prj.name}/src/**.cpp";

-- Simple copy folder to output
function symbol_table.CopyToOutput(folderName)
    return "{COPYDIR} " .. folderName .. " ../bin/" .. symbol_table.OutputDirectory .. "/%{prj.name}/" .. folderName;
end

return symbol_table;