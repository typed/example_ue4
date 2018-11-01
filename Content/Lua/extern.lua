local function clear_userdata(tbl)
    for k,v in pairs(tbl) do
        if type(v) == "userdata" then
            tbl[k] = nil
        end
    end
end
--Create an class.
function class(classname, url)
    local cls = {}
    cls.__classname = classname
    cls.construct   = function() end
    cls.destruct    = function() end
    function cls.new(...)
        local obj = {}
        setmetatable(obj, cls)
        cls.__index = cls
        obj.widget = slua.loadUI(url)
        obj.widget:SetLuaTable(obj)
        obj:construct(...)
        return obj
    end
    function cls:del()
        if self.widget then
            self:destruct()
            self.widget:SetLuaTable(nil)
            self.widget:RemoveFromViewport()
            self.widget = nil
            --clear_userdata(self)
        end
    end
    function cls.bind(wd,...)
        local obj = {}
        setmetatable(obj, cls)
        cls.__index = cls
        obj.widget = wd
        obj.widget:SetLuaTable(obj)
        obj:construct(...)
        return obj
    end
    function cls:unbind()
        if self.widget then
            self:destruct()
            self.widget:SetLuaTable(nil)
            --self.widget:RemoveFromViewport()
            self.widget = nil
            --clear_userdata(self)
        end
    end
    return cls
end