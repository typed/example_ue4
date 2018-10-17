local ScriptHelperClient = import('ScriptHelperClient')
ExampleReuseList = ExampleReuseList or {
    m_ui = nil,
}
function ExampleReuseList.Show()
    local ui = slua.loadUI('/Game/ExampleReuseList/ExampleReuseListUI.ExampleReuseListUI')
    ui:AddToViewport(0)
    ExampleReuseList.m_ui = ui
    local btnClose = ui:FindWidget("Button_Close")
    btnClose.OnClicked:Add(ExampleReuseList.OnClose)
end

function ExampleReuseList.Hide()
    if ExampleReuseList.m_ui then
        ExampleReuseList.m_ui:RemoveFromViewport()
        ExampleReuseList.m_ui = nil
    end
end

function ExampleReuseList.OnClose()
    ExampleReuseList.Hide()
end