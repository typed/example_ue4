
ExampleMain = ClassPanel("ExampleMain", '/Game/Example/ExampleMain/ExampleMainUI.ExampleMainUI')

function ExampleMain:construct()

    self.widget.Button_GC.OnClicked:Add(function() self:OnGC() end)

    self.widget.Button_ReuseList.OnClicked:Add(function() self:OnClick_Button_ReuseList() end)

    self.widget.Button_ReusePage.OnClicked:Add(function() self:OnClick_Button_ReusePage() end)

end

function ExampleMain:destruct()
    self.widget.Button_GC.OnClicked:Clear()
    self.widget.Button_ReuseList.OnClicked:Clear()
    self.widget.Button_ReusePage.OnClicked:Clear()
end

function ExampleMain:Trace()
    log("ExampleMain:Trace "..tostring(self.widget))
end

function ExampleMain:OnGC()
    --collectgarbage("collect")
    --SHC.GC()
    --SHC.TraceAllObject()
    --DumpSingleObjRef(self.widget)
    --MRI.m_cMethods.DumpMemorySnapshot("", "All", -1)
    --log_tree("debug.getregistry().SLUA_PTR_USERTABLE_MAPPING", debug.getregistry().SLUA_PTR_USERTABLE_MAPPING)
    SHC.TraceClass("/Game/Common/UICom.UICom_C")
    SHC.TraceClass("/Game/Example/ExampleMain/ExampleMainUI.ExampleMainUI_C")
end

function ExampleMain:OnClick_Button_ReuseList()
    local w = ExampleReuseList.new()
    w:Open()
end

function ExampleMain:OnClick_Button_ReusePage()
    local w = ExampleReusePage.new()
    w:Open()
end