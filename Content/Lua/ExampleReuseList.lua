local ScriptHelperClient = import('ScriptHelperClient')
ExampleReuseList = ExampleReuseList or {
    ui = nil,
    ReuseList = nil,
}
function ExampleReuseList.Show()
    local ui = slua.loadUI('/Game/ExampleReuseList/ExampleReuseListUI.ExampleReuseListUI')
    ui:AddToViewport(1)
    ExampleReuseList.ui = ui
    local Button_Close = ui:FindWidget("Button_Close")
    Button_Close.OnClicked:Add(ExampleReuseList.OnClose)

    local ButtonItem1 = ui:FindWidget("ButtonItem1")
    ButtonItem1.OnClicked:Add(ExampleReuseList.OnClickItem1)

    ExampleReuseList.ReuseList = ui:FindWidget("ReuseList")

end

function ExampleReuseList.Hide()
    if ExampleReuseList.ui then
        ExampleReuseList.ui:RemoveFromViewport()
        ExampleReuseList.ui = nil
    end
end

function ExampleReuseList.OnClose()
    ExampleReuseList.Hide()
    print("ExampleReuseList.OnClose")
end

function ExampleReuseList.OnClickItem1()
    local ReuseList = ExampleReuseList.ReuseList
    ReuseList.OnUpdateItem:Clear()
    ReuseList.OnUpdateItem:Add(ExampleReuseList.OnUpdateItem1)
    local itmClass = ScriptHelperClient.LoadUserWidgetClass("/Game/ExampleReuseList/TestReuseListItem.TestReuseListItem")
    --ReuseList:Test(itmClass)
    ReuseList:Reload(100, 100, 0, 0, itmClass, 0, 0)
end

function ExampleReuseList.OnUpdateItem1(widget,idx)
    print("OnUpdateItem1 idx="..idx)
end