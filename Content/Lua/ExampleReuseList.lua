


ExampleReuseList = class("ExampleReuseList", '/Game/ExampleReuseList/ExampleReuseListUI.ExampleReuseListUI')

function ExampleReuseList:ctor()

    self.ItmList = {}

    self.Button_Close = self.widget:FindWidget("Button_Close")
    self.Button_Close.OnClicked:Add(function() self:del() end)

    self.ButtonItem1 = self.widget:FindWidget("ButtonItem1")
    self.ButtonItem1.OnClicked:Add(function() self:OnClickItem1() end)

    self.ReuseList = self.widget:FindWidget("ReuseList")

end

function ExampleReuseList:Show()
    self.widget:AddToViewport(1)
end

function ExampleReuseList:OnClickItem1()
    
    self.ReuseList.OnUpdateItem:Clear()
    self.ReuseList.OnUpdateItem:Add(function(...) self:OnUpdateItem1(...) end)
    self.ReuseList.OnCreateItem:Clear()
    self.ReuseList.OnCreateItem:Add(function(...) self:OnCreateItem1(...) end)
    local itmClass = SHC.LoadClass("/Game/ExampleReuseList/TestReuseListItem.TestReuseListItem_C")
    self.ReuseList:Reload(100, 100, 0, 0, itmClass, 0, 0)
end

function ExampleReuseList:OnClickItem1BG(widget)
    local idx = widget:GetLuaTable().idx
    local itm = self.ItmList[idx]
    log("ExampleReuseList.OnClickItem1BG idx="..idx)
end

function ExampleReuseList:OnCreateItem1(widget)
    local function OnClick()
        self:OnClickItem1BG(widget)
    end
    widget.Button_BG.OnClicked:Add(OnClick)
    local tbl = {}
    widget:SetLuaTable(tbl)
end

function ExampleReuseList:OnUpdateItem1(widget,idx)
    widget:GetLuaTable().idx = idx
    local itmdata = self.ItmList[idx]
    --item:UpdateItem(ExampleReuseList.ItmData[idx].itm)
    --widget:GetBind():UpdateData(ExampleReuseList.ItmData[idx])
    widget.TextBlockName:SetText(idx)
    --Item.SetData(widget.Comonn_item, {})
end