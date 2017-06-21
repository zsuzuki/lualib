--[[
   lua test
   2017/06/20
]]
function co_test()
   for i = 1,8,1 do
      LOG(string.format("co_test: %x",Random.getInt()))
      coroutine.yield(0)
   end
   coroutine.yield(0)
   LOG("co_test done.")
   return 1
end

start_time = Time.Now()
LOG("Hello, World")
cr = coroutine.create(co_test)
coroutine.resume(cr)

r = coroutine.yield(0)
repeat
   local bStat, vRet = coroutine.resume(cr)
    if bStat == false then
        LOG(string.format("assert! -> %d",vRet))
    end
until coroutine.status(cr) == "dead"

end_time = Time.Now()
LOG(string.format("NowTime: %d",end_time - start_time))

Yield(0)

LOG("finish.")

return 0
-- End
