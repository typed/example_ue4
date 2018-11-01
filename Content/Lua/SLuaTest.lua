SHC = import("ScriptHelperClient")
MRI = require("memory_tool/MemoryReferenceInfo")
require("log_tree")
require("extern")
require("ExampleMain")
require("ExampleReuseList")
local w = ExampleMain.new() w:Open()
function Tick(dt,actor)
end

function DumpSingleObjRef(value)
    collectgarbage("collect")
    collectgarbage("collect")
    MRI.m_cMethods.DumpMemorySnapshotSingleObject(nil, "SingleObjRef-Object", -1, "value", value)
end