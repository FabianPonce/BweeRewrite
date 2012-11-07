http = require "socket.http"
http.USERAGENT = "Bwee IRC Bot by /u/motiveme"

-----------------------------------------------------------------------------------------
-- LUA only XmlParser from Alexander Makeev
-----------------------------------------------------------------------------------------
XmlParser = {};

function XmlParser:ToXmlString(value)
	value = string.gsub (value, "&", "&amp;");		-- '&' -> "&amp;"
	value = string.gsub (value, "<", "&lt;");		-- '<' -> "&lt;"
	value = string.gsub (value, ">", "&gt;");		-- '>' -> "&gt;"
	--value = string.gsub (value, "'", "&apos;");	-- '\'' -> "&apos;"
	value = string.gsub (value, "\"", "&quot;");	-- '"' -> "&quot;"
	-- replace non printable char -> "&#xD;"
   	value = string.gsub(value, "([^%w%&%;%p%\t% ])",
       	function (c) 
       		return string.format("&#x%X;", string.byte(c)) 
       		--return string.format("&#x%02X;", string.byte(c)) 
       		--return string.format("&#%02d;", string.byte(c)) 
       	end);
	return value;
end

function XmlParser:FromXmlString(value)
  	value = string.gsub(value, "&#x([%x]+)%;",
      	function(h) 
      		return string.char(tonumber(h,16)) 
      	end);
  	value = string.gsub(value, "&#([0-9]+)%;",
      	function(h) 
      		return string.char(tonumber(h,10)) 
      	end);
	value = string.gsub (value, "&quot;", "\"");
	value = string.gsub (value, "&apos;", "'");
	value = string.gsub (value, "&gt;", ">");
	value = string.gsub (value, "&lt;", "<");
	value = string.gsub (value, "&amp;", "&");
	return value;
end
   
function XmlParser:ParseArgs(s)
  local arg = {}
  string.gsub(s, "(%w+)=([\"'])(.-)%2", function (w, _, a)
    	arg[w] = self:FromXmlString(a);
  	end)
  return arg
end

function XmlParser:ParseXmlText(xmlText)
  local stack = {}
  local top = {Name=nil,Value=nil,Attributes={},ChildNodes={}}
  table.insert(stack, top)
  local ni,c,label,xarg, empty
  local i, j = 1, 1
  while true do
    ni,j,c,label,xarg, empty = string.find(xmlText, "<(%/?)([%w:]+)(.-)(%/?)>", i)
    if not ni then break end
    local text = string.sub(xmlText, i, ni-1);
    if not string.find(text, "^%s*$") then
      top.Value=(top.Value or "")..self:FromXmlString(text);
    end
    if empty == "/" then  -- empty element tag
      table.insert(top.ChildNodes, {Name=label,Value=nil,Attributes=self:ParseArgs(xarg),ChildNodes={}})
    elseif c == "" then   -- start tag
      top = {Name=label, Value=nil, Attributes=self:ParseArgs(xarg), ChildNodes={}}
      table.insert(stack, top)   -- new level
      --log("openTag ="..top.Name);
    else  -- end tag
      local toclose = table.remove(stack)  -- remove top
      --log("closeTag="..toclose.Name);
      top = stack[#stack]
      if #stack < 1 then
        error("XmlParser: nothing to close with "..label)
      end
      if toclose.Name ~= label then
        error("XmlParser: trying to close "..toclose.Name.." with "..label)
      end
      table.insert(top.ChildNodes, toclose)
    end
    i = j+1
  end
  local text = string.sub(xmlText, i);
  if not string.find(text, "^%s*$") then
      stack[#stack].Value=(stack[#stack].Value or "")..self:FromXmlString(text);
  end
  if #stack > 1 then
    error("XmlParser: unclosed "..stack[stack.n].Name)
  end
  return stack[1].ChildNodes[1];
end

function XmlParser:ParseXmlFile(xmlFileName)
	local hFile,err = io.open(xmlFileName,"r");
	if (not err) then
		local xmlText=hFile:read("*a"); -- read file content
		io.close(hFile);
        return self:ParseXmlText(xmlText),nil;
	else
		return nil,err;
	end
end
------------------------------------------------------------------------------------------

function printTable(t)

    function printTableHelper(t, spacing)
        for k,v in pairs(t) do
            print(spacing..tostring(k), v)
            if (type(v) == "table") then 
                printTableHelper(v, spacing.."\t")
            end
        end
    end

    printTableHelper(t, "");
end

NJNew = {
	LastUpdate = os.time(),
	
	CanPullXML = function()
		if os.difftime(os.time(), NJNew.LastUpdate) > 60 then
			return true
		else
			return false
		end
	end,
	
	GetXML = function()
		local content, statuscode, header = http.request("http://www.reddit.com/r/newjersey/new.xml?sort=new")
		if statuscode ~= 200 then
			return nil
		end
		NJNew.LastUpdate = os.time()
		return content
	end,
	
	GetPosts = function()
		local posts = {}
		local data = NJNew.GetXML()
		local tree = XmlParser:ParseXmlText(data)
		for i,node in pairs(tree.ChildNodes) do
			if(node.Name == "channel") then
				for i, subNode in pairs(node.ChildNodes) do
					if subNode.Name == "item" then
						local item = { }
						for i, itemNode in pairs(subNode.ChildNodes) do 
							if itemNode.Name == "title" then
								item.title = itemNode.Value
							elseif itemNode.Name == "link" then
								item.link = itemNode.Value
							elseif itemNode.Name == "pubDate" then
								item.date = itemNode.Value
							elseif itemNode.Name == "guid" then
								item.guid = itemNode.Value
							end
						end
						table.insert(posts, item)
					end
				end
			end
		end
		return posts
	end,
	
	SeenPosts = { },
	
	-- Returns ALL the new posts on r/newjersey/new!
	GetNewPosts = function()
		local posts = NJNew.GetPosts()
		local ret = {}
		for i, post in pairs(posts) do
			if NJNew.SeenPosts[post.guid] == nil then
				NJNew.SeenPosts[post.guid] = 1
				table.insert(ret, post)
			end
		end
		
		return ret
	end,
}

-- Run this once to remove all the new posts forevah!
NJNew.GetNewPosts()
