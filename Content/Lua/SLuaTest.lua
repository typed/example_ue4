SHC = import("ScriptHelperClient")
UICom = SHC.LoadBpClass("/Game/Common/UICom.UICom_C")
MRI = require("memory_tool/MemoryReferenceInfo")
require("log_tree")
require("extern")
require("ExampleMain")
require("ExampleReuseList")
require("ExampleReusePage")
local w = ExampleMain.new() w:Open()
function Tick(dt,actor)
end

function DumpSingleObjRef(value)
    collectgarbage("collect")
    collectgarbage("collect")
    MRI.m_cMethods.DumpMemorySnapshotSingleObject(nil, "SingleObjRef-Object", -1, "value", value)
end