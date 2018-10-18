local ScriptHelperClient = import('ScriptHelperClient')
ExampleMain = ExampleMain or {
    m_ui = nil,
}
function ExampleMain.Show()
    local ui = slua.loadUI('/Game/ExampleMain/ExampleMainUI.ExampleMainUI')
    ui:AddToViewport(0)
    ExampleMain.m_ui = ui

    local Button_GC = ui:FindWidget("Button_GC")
    Button_GC.OnClicked:Add(ExampleMain.OnGC)

    local Button_ReuseList = ui:FindWidget("Button_ReuseList")
    Button_ReuseList.OnClicked:Add(ExampleMain.OnClick_Button_ReuseList)

end

function ExampleMain.OnGC()
    ScriptHelperClient.GC()
end

function ExampleMain.OnClick_Button_ReuseList()
    ExampleReuseList.Show()
end