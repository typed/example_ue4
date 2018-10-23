ExampleReuseList = ExampleReuseList or {
    ui = nil,
    ReuseList = nil,
    ItmList = {},
}
function ExampleReuseList.Show()
    local ui = slua.loadUI('/Game/ExampleReuseList/ExampleReuseListUI.ExampleReuseListUI')
    --local ui = SHC.CreateUserWidget('/Game/ExampleReuseList/ExampleReuseListUI.ExampleReuseListUI_C')
    ui:AddToViewport(1)
    ExampleReuseList.ui = ui
    ui:SetLuaTable(ExampleReuseList)
    local Button_Close = ui:FindWidget("Button_Close")
    Button_Close.OnClicked:Add(ExampleReuseList.OnClose)

    local ButtonItem1 = ui:FindWidget("ButtonItem1")
    ButtonItem1.OnClicked:Add(ExampleReuseList.OnClickItem1)

    local ReuseList = ui:FindWidget("ReuseList")
    ExampleReuseList.ReuseList = ReuseList

end

function ExampleReuseList.Hide()
    if ExampleReuseList.ui then
        ExampleReuseList.ui:RemoveFromViewport()
        ExampleReuseList.ui = nil
    end
end

function ExampleReuseList.OnClose()
    ExampleReuseList.Hide()
    log("ExampleReuseList.OnClose")
end

function ExampleReuseList.OnClickItem1()
    local ReuseList = ExampleReuseList.ReuseList
    ReuseList.OnUpdateItem:Clear()
    ReuseList.OnUpdateItem:Add(ExampleReuseList.OnUpdateItem1)
    ReuseList.OnCreateItem:Clear()
    ReuseList.OnCreateItem:Add(ExampleReuseList.OnCreateItem1)
    local itmClass = SHC.LoadClass("/Game/ExampleReuseList/TestReuseListItem.TestReuseListItem_C")
    ReuseList:Reload(100, 100, 0, 0, itmClass, 0, 0)
    --ReuseList:SetBind(ExampleReuseList.ItmData)
    --ReuseList:SetList(ExampleReuseList.ItmData)
end

function ExampleReuseList.OnClickItem1BG(widget)
    log("ExampleReuseList.OnClickItem1BG idx="..widget:GetLuaTable().idx)
end

function ExampleReuseList.OnCreateItem1(widget)
    local function OnClick()
        ExampleReuseList.OnClickItem1BG(widget)
    end
    widget.Button_BG.OnClicked:Add(OnClick)
    widget:SetLuaTable({})
end

function ExampleReuseList.OnUpdateItem1(widget,idx)
    widget:GetLuaTable().idx = idx
    --item:UpdateItem(ExampleReuseList.ItmData[idx].itm)
    --widget:GetBind():UpdateData(ExampleReuseList.ItmData[idx])
    widget.TextBlockName:SetText(idx)
    local itm = ExampleReuseList.ItmList[idx]
    --Item.SetData(widget.Comonn_item, {})
end