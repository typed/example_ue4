
ExampleMain = ClassPanel("ExampleMain", '/Game/Example/ExampleMain/ExampleMainUI.ExampleMainUI')

function ExampleMain:construct()
    self.widget.Button_GC.OnClicked:Add(function() self:OnGC() end)
    self.widget.Button_ReuseList.OnClicked:Add(function() self:OnClick_Button_ReuseList() end)
    self.widget.Button_ReusePage.OnClicked:Add(function() self:OnClick_Button_ReusePage() end)
    self.widget.Button_Shape.OnClicked:Add(function() self:OnClick_Button_Shape() end)
end

function ExampleMain:destruct()
    --[[
    self.widget.Button_GC.OnClicked:Clear()
    self.widget.Button_ReuseList.OnClicked:Clear()
    self.widget.Button_ReusePage.OnClicked:Clear()
    self.widget.Button_Shape.OnClicked:Clear()
    ]]
end

function ExampleMain:Trace()
    log("ExampleMain:Trace "..tostring(self.widget))
end

function ExampleMain:OnGC()
    collectgarbage("collect")
    US.GC()
    US.TraceAllObject()
    --DumpSingleObjRef(self.widget)
    --MRI.m_cMethods.DumpMemorySnapshot("", "All", -1)
    --log_tree("debug.getregistry().SLUA_PTR_USERTABLE_MAPPING", debug.getregistry().SLUA_PTR_USERTABLE_MAPPING)
    --US.TraceClass("/Game/Common/UICom.UICom_C")
    --US.TraceClass("/Game/Example/ExampleReusePage/ExampleReusePageUI.ExampleReusePageUI_C")
end

function ExampleMain:OnClick_Button_ReuseList()
    ExampleReuseList.new():Open()
end

function ExampleMain:OnClick_Button_ReusePage()
    ExampleReusePage.new():Open()
end

function ExampleMain:OnClick_Button_Shape()
    ExampleRadarChart.new():Open()
end