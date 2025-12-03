function sysCall_init() 
    openCloseMotorHandle=sim.getObject('./openCloseJoint')
    centerJoint=sim.getObject('./centerJoint')
    sim.setInt32Signal('gripperClosed', 0)
    end

function sysCall_actuation() 
    local close = sim.getInt32Signal('gripperClosed')
    if (close==1) then
        sim.setJointTargetVelocity(openCloseMotorHandle,-0.15) --original 0.02
    else
        sim.setJointTargetVelocity(openCloseMotorHandle,0.15)
    end
end 

function sysCall_joint(inData)
    if inData.handle==centerJoint then
        local desired=sim.getJointPosition(openCloseMotorHandle)/2
        local error=desired-inData.pos
        local ctrl=error*20
        
        local maxVelocity=ctrl
        if (maxVelocity>inData.maxVel) then
            maxVelocity=inData.maxVel
        end
        if (maxVelocity<-inData.maxVel) then
            maxVelocity=-inData.maxVel
        end
        local forceOrTorqueToApply=inData.maxForce

        local outData={vel=maxVelocity,force=forceOrTorqueToApply}
        return outData
    end
end