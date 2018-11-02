


ExampleReuseList = ClassPanel("ExampleReuseList", '/Game/Example/ExampleReuseList/ExampleReuseListUI.ExampleReuseListUI')

function ExampleReuseList:construct()

    self.ItmList = {}
    self.Item1WidgetList = {}

    UICom.SetImage("/Game/Texture/UMG/wupingicon_baiyingjianzhi.wupingicon_baiyingjianzhi", self.widget.Image1)

    self.widget.Button_Close.OnClicked:Add(function() self:Close() end)

    self.widget.ButtonItem1.OnClicked:Add(function() self:OnClickItem1() end)

    self.widget.Button_4.OnClicked:Add(function() self:OnClear() end)

    self.widget.Button_0.OnClicked:Add(function() self:OnJumpByIdx() end)

    --[[
    self.ReuseListC.OnUpdateItem:Clear()
    self.ReuseListC.OnUpdateItem:Add(function(...) self:OnUpdateItem1(...) end)
    self.ReuseListC.OnCreateItem:Clear()
    self.ReuseListC.OnCreateItem:Add(function(...) self:OnCreateItem1(...) end)
    local itmClass = SHC.LoadClass("/Game/ExampleReuseList/TestReuseListItem.TestReuseListItem_C")
    self.ReuseListC:Reload(1000, 100, 0, 0, itmClass, 0, 0, true)
    ]]

    

end

function ExampleReuseList:destruct()
    self.widget.Button_0.OnClicked:Clear()
    self.widget.Button_4.OnClicked:Clear()
    self.widget.Button_Close.OnClicked:Clear()
    self.widget.ButtonItem1.OnClicked:Clear()
    self.widget.ReuseListC.OnUpdateItem:Clear()
    self.widget.ReuseListC.OnCreateItem:Clear()
    for i,v in ipairs(self.Item1WidgetList) do
        v:unbind()
    end
    self.Item1WidgetList = {}
end

function ExampleReuseList:OnJumpByIdx()
    self.widget.ReuseListC:JumpByIdx(22,0)
end

function ExampleReuseList:OnClickItem1()
    self.widget.ReuseListC.OnUpdateItem:Clear()
    self.widget.ReuseListC.OnUpdateItem:Add(function(...) self:OnUpdateItem1(...) end)
    self.widget.ReuseListC.OnCreateItem:Clear()
    self.widget.ReuseListC.OnCreateItem:Add(function(...) self:OnCreateItem1(...) end)
    local itmClass = SHC.LoadBpClass("/Game/Example/ExampleReuseList/TestReuseListItem.TestReuseListItem_C")
    self.widget.ReuseListC:Reload(5000, 100, 0, 0, itmClass, 0, 0, true)
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

function ExampleReuseList:OnClear()
    self.widget.ReuseListC:Clear()
    --DumpSingleObjRef(self)
    --log_tree("debug.getregistry().SLUA_PTR_USERTABLE_MAPPING", debug.getregistry().SLUA_PTR_USERTABLE_MAPPING)
end

TestReuseListItem = class("TestReuseListItem", "/Game/Example/ExampleReuseList/TestReuseListItem.TestReuseListItem")

function TestReuseListItem:construct(parent)
    self.m_parent = parent
    self.m_idx = 0
    self.widget.Button_BG.OnClicked:Add(function() self:OnClickItem1BG() end)
    --self.ImageIcon = self.widget:FindWidget("ImageIcon")
end
function TestReuseListItem:destruct()
    self.widget.Button_BG.OnClicked:Clear()
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