ExampleMain = ExampleMain or {
    ui = nil,
}

function ExampleMain:Trace()
    log("ExampleMain:Trace "..tostring(self.ui))
end

function ExampleMain.Show()
    local ui = slua.loadUI('/Game/ExampleMain/ExampleMainUI.ExampleMainUI')
    --local ui = SHC.CreateUserWidget('/Game/ExampleMain/ExampleMainUI.ExampleMainUI_C')
    ui:AddToViewport(0)
    
    ExampleMain.ui = ui

    --[[
    local tbl = {
        name = "aaa",
        age = 11,
        list = {
            [1] = {n1=1, n2=100},
            [2] = {n1=2, n2=200},
        }
    }
    ]]
    ui:SetLuaTable(ExampleMain)

    local Button_GC = ui:FindWidget("Button_GC")
    Button_GC.OnClicked:Clear()
    Button_GC.OnClicked:Add(ExampleMain.OnGC)

    local Button_ReuseList = ui:FindWidget("Button_ReuseList")
    Button_ReuseList.OnClicked:Clear()
    Button_ReuseList.OnClicked:Add(ExampleMain.OnClick_Button_ReuseList)

end

function ExampleMain.OnGC()
    SHC.GC()
    SHC.TraceAllObject()
    collectgarbage("collect")
end

function ExampleMain.OnClick_Button_ReuseList()
    log_tree("ui:GetLuaTable", ExampleMain.ui:GetLuaTable())
    ExampleReuseList.Show()
end