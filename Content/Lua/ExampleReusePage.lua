
ExampleReusePage = ClassPanel("ExampleReusePage", "/Game/Example/ExampleReusePage/ExampleReusePageUI.ExampleReusePageUI")
function ExampleReusePage:construct()
    self.widget.Button_Close.OnClicked:Add(function() self:Close() end)
    self.widget.Button_AniPage.OnClicked:Add(function() self.widget.ReusePageC:MoveNextPage() end)
    self.widget.Button_SetPageRand.OnClicked:Add(function() self.widget.ReusePageC:SetPage(math.random(self.page_count-1)) end)
    self.widget.Button_ReloadCount0.OnClicked:Add(function() self.widget.ReusePageC:Reload(0) end)
    self.widget.Button_ReloadCountRand.OnClicked:Add(function() self:OnReloadCountRand() end)
end

function ExampleReusePage:destruct()
    self.widget.Button_Close.OnClicked:Clear()
    self.widget.Button_ReloadCountRand.OnClicked:Clear()
    self.widget.ReusePageC.OnCreateItem:Clear()
    self.widget.ReusePageC.OnUpdateItem:Clear()
    self.widget.ReusePageC.OnPageChanged:Clear()
end

function ExampleReusePage:OnReloadCountRand()
    self.widget.ReusePageC.OnCreateItem:Clear()
    self.widget.ReusePageC.OnCreateItem:Add(function(widget)
        TestReusePageItem.bind(widget, self)
    end)
    self.widget.ReusePageC.OnUpdateItem:Clear()
    self.widget.ReusePageC.OnUpdateItem:Add(function(widget, idx)
        widget:GetLuaTable():UpdateData(idx)
    end)
    self.widget.ReusePageC.OnPageChanged:Clear()
    self.widget.ReusePageC.OnPageChanged:Add(function(page_idx)
        log("OnPageChanged page_idx="..page_idx)
    end)
    self.page_count = math.random(100)
    self.widget.ReusePageC:Reload(self.page_count)
    self.widget.ReusePageC:SetPage(0)
end

TestReusePageItem = class("TestReusePageItem", "/Game/Example/ExampleReuseList/TestReusePageItem.TestReusePageItem")
function TestReusePageItem:construct(parent)
    self.m_parent = parent
    self.m_idx = 0
end
function TestReusePageItem:destruct()
    
end
function TestReusePageItem:UpdateData(idx)
    self.m_idx = idx
    self.widget:SetData(idx)
end