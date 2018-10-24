


ExampleReuseList = class("ExampleReuseList", '/Game/ExampleReuseList/ExampleReuseListUI.ExampleReuseListUI')

function ExampleReuseList:ctor()

    self.ItmList = {}

    self.Button_Close = self.widget:FindWidget("Button_Close")
    self.Button_Close.OnClicked:Add(function() self:Hide() end)

    self.ButtonItem1 = self.widget:FindWidget("ButtonItem1")
    self.ButtonItem1.OnClicked:Add(function() self:OnClickItem1() end)

    self.ReuseList = self.widget:FindWidget("ReuseList")
    self.Item1WidgetList = {}

end

function ExampleReuseList:Show()
    self.widget:AddToViewport(1)
end

function ExampleReuseList:Hide()
    self.Button_Close.OnClicked:Clear()
    self.ButtonItem1.OnClicked:Clear()
    self.ReuseList.OnUpdateItem:Clear()
    self.ReuseList.OnCreateItem:Clear()
    for i,v in ipairs(self.Item1WidgetList) do
        v:unbind()
    end
    self:del()
end

function ExampleReuseList:OnClickItem1()
    self.ReuseList.OnUpdateItem:Clear()
    self.ReuseList.OnUpdateItem:Add(function(...) self:OnUpdateItem1(...) end)
    self.ReuseList.OnCreateItem:Clear()
    self.ReuseList.OnCreateItem:Add(function(...) self:OnCreateItem1(...) end)
    local itmClass = SHC.LoadClass("/Game/ExampleReuseList/TestReuseListItem.TestReuseListItem_C")
    self.ReuseList:Reload(100, 100, 0, 0, itmClass, 0, 0)
end

function ExampleReuseList:OnCreateItem1(widget)
    local itm_widget = TestReuseListItem.bind(widget, self)
    table.insert(self.Item1WidgetList, itm_widget)
end

function ExampleReuseList:OnUpdateItem1(widget,idx)
    local itm_widget = widget:GetLuaTable()
    itm_widget:SetIdx(idx)
    itm_widget:UpdateData()
end

TestReuseListItem = class("TestReuseListItem", "/Game/ExampleReuseList/TestReuseListItem.TestReuseListItem")

function TestReuseListItem:ctor(parent)
    self.m_parent = parent
    self.m_idx = 0
    --self.ItmList = {}
    self.widget.Button_BG.OnClicked:Add(function() self:OnClickItem1BG() end)
end
function TestReuseListItem:SetIdx(idx)
    self.m_idx = idx
end
function TestReuseListItem:GetIdx()
    return self.m_idx
end
function TestReuseListItem:OnClickItem1BG()
    local idx = self.m_idx
    --local itm = self.ItmList[idx]
    log("ExampleReuseList.OnClickItem1BG idx="..idx)
end
function TestReuseListItem:UpdateData()
    self.widget.TextBlockName:SetText(self.m_idx)
end