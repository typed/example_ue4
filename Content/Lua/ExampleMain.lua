
ExampleMain = class("ExampleMain", '/Game/ExampleMain/ExampleMainUI.ExampleMainUI')

function ExampleMain:ctor()

    local Button_GC = self.widget:FindWidget("Button_GC")
    Button_GC.OnClicked:Add(function() self:OnGC() end)

    local Button_ReuseList = self.widget:FindWidget("Button_ReuseList")
    Button_ReuseList.OnClicked:Add(function() self:OnClick_Button_ReuseList() end)

end

function ExampleMain:Trace()
    log("ExampleMain:Trace "..tostring(self.widget))
end

function ExampleMain:Show()
    self.widget:AddToViewport(0)
end

function ExampleMain:OnGC()
    SHC.GC()
    SHC.TraceAllObject()
    collectgarbage("collect")
    MRI.m_cMethods.DumpMemorySnapshotSingleObject(nil, "SingleObjRef-Object", -1, "widget", self.widget)
end

function ExampleMain:OnClick_Button_ReuseList()
    local w = ExampleReuseList.new()
    w:Show()
end