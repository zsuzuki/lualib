--[[
   lua test
   2017/06/20
]]
function co_test()
   for i = 1,8,1 do
      LOG.CHECK(string.format("co_test: %x",Random.getInt()))
      coroutine.yield(0)
   end
   coroutine.yield(0)
   LOG.CHECK("co_test done.")
   return 1
end

lv = tonumber(ARGS["TLEVEL"])
if lv then
    LOG.LEVEL(lv)
end

t = TEST.new(8)
t:print(2)
LOG.NORMAL(t.num)
LOG.NORMAL(t.real)
LOG.NORMAL(t.b)
t:print(3)
start_time = Time.Now()
LOG.NORMAL("Hello","World")
cr = coroutine.create(co_test)
coroutine.resume(cr)

r = coroutine.yield(0)
repeat
   local bStat, vRet = coroutine.resume(cr)
    if bStat == false then
        LOG.NORMAL(string.format("assert! -> %d",vRet))
    end
until coroutine.status(cr) == "dead"

end_time = Time.Now()
LOG.NORMAL("Time:",string.format("NowTime: %d",end_time - start_time))
LOG.NORMAL("CPU: ",os.clock())

Yield(0)

LOG.NORMAL("finish.")

return 0
-- End
