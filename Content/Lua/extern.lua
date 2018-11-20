
--Create an class.
function class(classname, url)
    local cls = {}
    cls.__classname = classname
    cls.construct   = function() end
    cls.destruct    = function() end
    cls.__destruct  = function(obj)
        --log("__destruct!!!!!!!!!"..tostring(obj))
        obj:destruct()
        obj.widget = nil
    end
    function cls.new(...)
        local obj = {}
        setmetatable(obj, cls)
        cls.__index = cls
        obj.__create_type = "new"
        obj.widget = slua.loadUI(url)
        obj.widget:SetLuaTable(obj)
        obj:construct(...)
        return obj
    end
    function cls:del()
        if self.widget then
            self.widget:RemoveFromViewport()
        end
    end
    function cls.bind(wd,...)
        local obj = {}
        setmetatable(obj, cls)
        cls.__index = cls
        obj.__create_type = "bind"
        obj.widget = wd
        obj.widget:SetLuaTable(obj)
        obj:construct(...)
        return obj
    end
    return cls
end

local g_lastOrder = -1
local g_lstPanel = {}
local g_maxPanel = 4
function ClassPanel(classname, url)
    local cls = class(classname, url)
    function cls:Close()
        self:del()
        for i,v in ipairs(g_lstPanel) do
            if v == self then
                if self.__order == g_lastOrder then
                    g_lastOrder = g_lastOrder - 1
                end
                table.remove(g_lstPanel, i)
                return
            end
        end
    end
    function cls:Open()
        g_lastOrder = g_lastOrder + 1
        self.__order = g_lastOrder
        self.widget:AddToViewport(g_lastOrder)
        table.insert(g_lstPanel, self)
        if #g_lstPanel >= g_maxPanel then
            g_lstPanel[1]:Close()
        end
    end
    return cls
end

function CloseAllPanel()
    for i,v in ipairs(g_lstPanel) do
        v:Close()
    end
    g_lastOrder = -1
    g_lstPanel = {}
end