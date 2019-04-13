


ExampleReuseList = ClassPanel("ExampleReuseList", '/Game/Example/ExampleReuseList/ExampleReuseListUI.ExampleReuseListUI')
function ExampleReuseList:construct()
    self.ItmList = {}
    --UICom.SetImage("/Game/Texture/UMG/wupingicon_baiyingjianzhi.wupingicon_baiyingjianzhi", self.widget.Image1)
    self.widget.Button_Close.OnClicked:Add(function() self:Close() end)
    self.widget.ButtonItem1.OnClicked:Add(function() self:OnClickItem1() end)
    self.widget.Button_4.OnClicked:Add(function() self:OnClear() end)
    self.widget.Button_0.OnClicked:Add(function() self:OnJumpByIdx() end)
    self:OnClickItem1()
end

function ExampleReuseList:destruct()
    log("ExampleReuseList:destruct")
    self.widget.Button_0.OnClicked:Clear()
    self.widget.Button_4.OnClicked:Clear()
    self.widget.Button_Close.OnClicked:Clear()
    self.widget.ButtonItem1.OnClicked:Clear()
    self.widget.ReuseList2.OnUpdateItem:Clear() 
    self.widget.ReuseList2.OnCreateItem:Clear()
end

function ExampleReuseList:OnJumpByIdx()
    self.widget.ReuseList2:JumpByIdx(22,0)
end

function ExampleReuseList:OnClickItem1()
    self.widget.ReuseList2.OnUpdateItem:Clear()
    self.widget.ReuseList2.OnUpdateItem:Add(function(...) self:OnUpdateItem1(...) end)
    self.widget.ReuseList2.OnCreateItem:Clear()
    self.widget.ReuseList2.OnCreateItem:Add(function(widget) TestReuseListItem.bind(widget, self) end)
    --local cls = US.LoadBpClass("/Game/Example/ExampleReuseList/TestReuseListItem3.TestReuseListItem3_C")
    --self.widget.ReuseList2:Reset(cls, 2, 100, 100, 5, 5)
    self.widget.ReuseList2:Reload(math.random(1000))
    --self.widget.ReuseList2:Reload(3)
end

function ExampleReuseList:OnUpdateItem1(widget,idx)
    log("TestReuseListItem idx="..idx.." widget="..tostring(widget))
    local itm_widget = widget:GetLuaTable()
    itm_widget:UpdateData(idx)
end

function ExampleReuseList:OnClear()
    self.widget.ReuseList2:Clear()
    --DumpSingleObjRef(self)
    --log_tree("debug.getregistry().SLUA_PTR_USERTABLE_MAPPING", debug.getregistry().SLUA_PTR_USERTABLE_MAPPING)
end

--ExampleReuseList的子控件
TestReuseListItem = class("TestReuseListItem", "/Game/Example/ExampleReuseList/TestReuseListItem.TestReuseListItem")
function TestReuseListItem:construct(parent)
    self.m_parent = parent
    self.m_idx = 0
    self.widget.Button_BG.OnClicked:Add(function() self:OnClickItem1BG() end)
    self.widget.ReuseList2.OnUpdateItem:Clear()
    self.widget.ReuseList2.OnUpdateItem:Add(function(...) self:OnUpdateItem(...) end)
    self.widget.ReuseList2.OnCreateItem:Clear()
    self.widget.ReuseList2.OnCreateItem:Add(function(widget) TestReuseListItem3.bind(widget, self) end)
    --self.widget.ReuseList2:Reload(math.random(10))
end
function TestReuseListItem:destruct()
    self.widget.Button_BG.OnClicked:Clear()
    self.widget.ReuseList2.OnUpdateItem:Clear()
    self.widget.ReuseList2.OnCreateItem:Clear()
end
function TestReuseListItem:OnClickItem1BG()
    --local itm = self.ItmList[self.m_idx]
    log("TestReuseListItem idx="..self.m_idx)
end
function TestReuseListItem:UpdateData(idx)
    self.m_idx = idx
    self.widget:SetData(self.m_idx)
end
function TestReuseListItem:OnUpdateItem(widget,idx)
    widget:GetLuaTable():UpdateData(idx)
end

--TestReuseListItem的子控件
TestReuseListItem3 = class("TestReuseListItem3", "/Game/Example/ExampleReuseList/TestReuseListItem3.TestReuseListItem3")
function TestReuseListItem3:construct(parent)
    self.m_parent = parent
    self.m_idx = 0
    self.widget.Button_103.OnClicked:Add(function() self:OnClick() end)
end
function TestReuseListItem3:destruct()
    self.widget.Button_103.OnClicked:Clear()
end
function TestReuseListItem3:UpdateData(idx)
    self.m_idx = idx
    self.widget:SetData(self.m_idx)
end
function TestReuseListItem3:OnClick()
    --local itm = self.ItmList[self.m_idx]
    log("TestReuseListItem idx="..self.m_parent.m_idx.." TestReuseListItem3 idx="..self.m_idx)
end