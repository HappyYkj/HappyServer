local redis = require 'redis'

local params = {
    host = '127.0.0.1',
    port = 6379,
}

local client = redis.connect(params)
client:auth('syg23333')
-- client:select(15) -- for testing purposes

local co = coroutine.create(function()
	local num = 0
	local ti = os.time()
	while true do
		client:lpush('foo1', 'bar')
		local value = client:brpop('foo1', 3)
		num = num + 1

		if num % 10000 == 0 then
			num = 0
			print(">>>>>>>>>>> 10000 cost ti", os.time() - ti)
			ti = os.time()
		end
	end
end)
coroutine.resume(co)
