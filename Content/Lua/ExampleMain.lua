ExampleMain = ExampleMain or {
}
function ExampleMain.Show()
    local ui = slua.loadUI('/Game/ExampleMain/ExampleMainUI.ExampleMainUI')
    --local ui = SHC.CreateUserWidget('/Game/ExampleMain/ExampleMainUI.ExampleMainUI_C')
    
    ui:AddToViewport(0)

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
    ExampleReuseList.Show()
end