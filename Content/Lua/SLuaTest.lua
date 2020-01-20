math.randomseed(os.time())
US = import("UtilScript")
UICom = US.LoadBpClass("/Game/Common/UICom.UICom_C")
MRI = require("memory_tool/MemoryReferenceInfo")
require("log_tree")
require("extern")
require("ExampleMain")
require("ExampleReuseList")
require("ExampleReusePage")
require("ExampleRadarChart")
ExampleMain.new():Open()
function Tick(dt,actor)
end

function DumpSingleObjRef(value)
    collectgarbage("collect")
    collectgarbage("collect")
    MRI.m_cMethods.DumpMemorySnapshotSingleObject(nil, "SingleObjRef-Object", -1, "value", value)
end