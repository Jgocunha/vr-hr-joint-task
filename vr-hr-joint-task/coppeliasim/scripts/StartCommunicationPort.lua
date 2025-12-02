function sysCall_init()
    corout=coroutine.create(coroutineMain)
    
    --simRemoteApi.start(19997)
    simRemoteApi.start(19995)
end

function sysCall_actuation()
    if coroutine.status(corout)~='dead' then
        local ok,errorMsg=coroutine.resume(corout)
        if errorMsg then
            error(debug.traceback(corout,errorMsg),2)
        end
    end
end

function sysCall_cleanup()
    -- do some clean-up here
end

function coroutineMain()

end



-- See the user manual or the available code snippets for additional callback functions and details
