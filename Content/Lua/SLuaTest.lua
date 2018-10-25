SHC = import('ScriptHelperClient')
require 'log_tree'
MRI = require("memory_tool/MemoryReferenceInfo")
require 'extern'
require 'ExampleMain'
require 'ExampleReuseList'
local w = ExampleMain.new() w:Show()
function Tick(dt,actor)
end
