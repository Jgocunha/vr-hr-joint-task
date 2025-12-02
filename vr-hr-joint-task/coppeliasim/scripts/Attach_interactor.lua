function __setObjectPosition__(a,b,c)
    -- compatibility routine, wrong results could be returned in some situations, in CoppeliaSim <4.0.1
    if b==sim.handle_parent then
        b=sim.getObjectParent(a)
    end
    if (b~=-1) and (sim.getObjectType(b)==sim.object_joint_type) and (sim.getInt32Param(sim.intparam_program_version)>=40001) then
        a=a+sim.handleflag_reljointbaseframe
    end
    return sim.setObjectPosition(a,b,c)
end
function __getObjectPosition__(a,b)
    -- compatibility routine, wrong results could be returned in some situations, in CoppeliaSim <4.0.1
    if b==sim.handle_parent then
        b=sim.getObjectParent(a)
    end
    if (b~=-1) and (sim.getObjectType(b)==sim.object_joint_type) and (sim.getInt32Param(sim.intparam_program_version)>=40001) then
        a=a+sim.handleflag_reljointbaseframe
    end
    return sim.getObjectPosition(a,b)
end
-- Make this interactor a child of any controller (right or left does not matter). 
-- You can attatch any object of which the handle is included in handles which is closest
-- to the controller by holding the grip button. Release the object by performing the same operation
-- You can also copy the interactor and attatch the copy to the other controller

-- Possible modifications:  - Change the objects that can be draged by changeing the handles variable
--                          - Change the action on buttonpress event

-- All available signals are displayed at the bottom of this script

function sysCall_init()
    handles = {}
    -- Generate an array of handles that can be attatched
    --handles = sim.getObjectsInTree(sim.handle_scene, sim.object_shape_type, 0) -- all shapes
    -- handles[1] = sim.getObjectHandle('A_single_object') the tip of a robot maybe?
    -- handles = {sim.getObjectHandle('object_1'), ..., sim.getObjectHandle('object_n')}

    maxPickDist = 0.12
    -- Detect if the interactor is attatched to the left/right/ or none at all controller
    detectParent()
    attached = false
    dragH = -1
    count = 0
    
    sim.setInt32Signal("humanGraspObj1", 0)
    sim.setInt32Signal("humanGraspObj2", 0)
    sim.setInt32Signal("humanGraspObj3", 0)
    sim.setInt32Signal("humanGraspObj4", 0)
    sim.setInt32Signal("humanGraspObj5", 0)
    sim.setInt32Signal("humanGraspObj6", 0)
    sim.setInt32Signal("humanGraspObj7", 0)
    sim.setInt32Signal("humanGraspObj8", 0)
   
    
    sim.setInt32Signal("humanPlaceObj1", 0)
    sim.setInt32Signal("humanPlaceObj2", 0)
    sim.setInt32Signal("humanPlaceObj3", 0)
    sim.setInt32Signal("humanPlaceObj4", 0)
    sim.setInt32Signal("humanPlaceObj5", 0)
    sim.setInt32Signal("humanPlaceObj6", 0)
    sim.setInt32Signal("humanPlaceObj7", 0)
    sim.setInt32Signal("humanPlaceObj8", 0)
    
    
    releasePositionBounds = {
    min_x = -0.1,
    max_x = 0.1,
    min_y = 0.510,
    max_y = 0.710,
    min_z = 0.0,
    max_z = 1.2
    }

end


function isReleasePositionValid(position)
    -- Check if the release position falls within the specified bounds
    return position[1] >= releasePositionBounds.min_x and position[1] <= releasePositionBounds.max_x and
           position[2] >= releasePositionBounds.min_y and position[2] <= releasePositionBounds.max_y and
           position[3] >= releasePositionBounds.min_z and position[3] <= releasePositionBounds.max_z
end

function sysCall_actuation()
    -- If this interactor is active
    if active==false then
        return
    end

    -- see if objects have been created
    local objectsCreated = sim.getInt32Signal('objectsCreated')
    if objectsCreated == 8 then
        handles[1] = sim.getObjectHandle('object_1')
        handles[2] = sim.getObjectHandle('object_2')
        handles[3] = sim.getObjectHandle('object_3')
        handles[4] = sim.getObjectHandle('object_4')
        handles[5] = sim.getObjectHandle('object_5')
        handles[6] = sim.getObjectHandle('object_6')
        handles[7] = sim.getObjectHandle('object_7')
        handles[8] = sim.getObjectHandle('object_8')
        
    end
    
    -- Detect butonpress
    grip = (sim.getInt32Signal(prefix .. 'Grip_Press')==1)
    trigger = (sim.getInt32Signal(prefix .. 'Trigger_Press')==1)

    -- Define action
    -- !!!  Put you're code here    !!! --
    action()
    -- if trigger then
    --     Do whatever you want
    -- end
end

function action()
    -- Check if the grip is active
    if (trigger == true) then
        -- Increment the count
        count = count + 1
        -- If count reaches 10 and an object is not already attached
        if (count == 3) and (attached == false) then
            -- Detect the nearest object
            dragH = detectNearestObject()
            -- Check if an object is detected
            if not (dragH == -1) then            
                -- Depending on the nearest object, set corresponding signal
                if (dragH == handles[1]) then
                    targetObject = 1
                elseif (dragH == handles[2]) then
                    targetObject = 2
                elseif (dragH == handles[3]) then
                    targetObject = 3
                elseif (dragH == handles[4]) then
                    targetObject = 4
                elseif (dragH == handles[5]) then
                    targetObject = 5
                elseif (dragH == handles[6]) then
                    targetObject = 6
                elseif (dragH == handles[7]) then
                    targetObject = 7
                elseif (dragH == handles[8]) then
                    targetObject = 8
                
                end
                -- Check if the object is available for grasping
                if sim.getInt32Signal("robotGraspObj"..targetObject) == 0 then
                    sim.setInt32Signal("humanGraspObj"..targetObject, 1)
                    -- Attach the object to the parent
                    sim.setObjectParent(dragH, parent, true)
                    sim.setObjectInt32Param(dragH, sim.shapeintparam_respondable, 0)
                    sim.setObjectInt32Param(dragH, sim.shapeintparam_static, 1)
                    attached = true
                end
            end
        end
    else
        -- If count reaches 10 and an object is already attached
        --elseif (count == 3) and (attached == true) then
        if attached == true then
            -- Release the attached object
            if not (dragH == -1) then
                attached = false
                if sim.getInt32Signal("robotGraspObj"..targetObject) == 0 then
                    -- Detach the object
                    if sim.getObjectParent(dragH) ~= -1 then
                        sim.setObjectParent(dragH, -1, true)
                        sim.setObjectInt32Param(dragH, sim.shapeintparam_respondable, 1)
                        sim.setObjectInt32Param(dragH, sim.shapeintparam_static, 0)
                        sim.resetDynamicObject(dragH)
                    end
                    -- Get the position of the released object
                    local releasePosition = sim.getObjectPosition(dragH, -1)

                    -- Check if the release position is valid
                    if isReleasePositionValid(releasePosition) then
                        -- Set the corresponding signal based on the released object
                        if dragH == handles[1] then
                            sim.setInt32Signal("humanPlaceObj1", 1)
                        elseif dragH == handles[2] then
                            sim.setInt32Signal("humanPlaceObj2", 1)
                        elseif dragH == handles[3] then
                            sim.setInt32Signal("humanPlaceObj3", 1)
                        elseif dragH == handles[4] then
                            sim.setInt32Signal("humanPlaceObj4", 1)
                        elseif dragH == handles[5] then
                            sim.setInt32Signal("humanPlaceObj5", 1)
                        elseif dragH == handles[6] then
                            sim.setInt32Signal("humanPlaceObj6", 1)
                        elseif dragH == handles[7] then
                            sim.setInt32Signal("humanPlaceObj7", 1)
                        elseif dragH == handles[8] then
                            sim.setInt32Signal("humanPlaceObj8", 1)
                        
                        end
                    else
                        -- Reset the corresponding grasp signal if the release position is not valid
                        if dragH == handles[1] then
                            sim.setInt32Signal("humanGraspObj1", 0)
                        elseif dragH == handles[2] then
                            sim.setInt32Signal("humanGraspObj2", 0)
                        elseif dragH == handles[3] then
                            sim.setInt32Signal("humanGraspObj3", 0)
                        elseif dragH == handles[4] then
                            sim.setInt32Signal("humanGraspObj4", 0)
                        elseif dragH == handles[5] then
                            sim.setInt32Signal("humanGraspObj5", 0)
                        elseif dragH == handles[6] then
                            sim.setInt32Signal("humanGraspObj6", 0)
                        elseif dragH == handles[7] then
                            sim.setInt32Signal("humanGraspObj7", 0)
                        elseif dragH == handles[8] then
                            sim.setInt32Signal("humanGraspObj8", 0)
                       
                        end
                    end
                end
                dragH = -1
            end
        end
        count = 0
    end
    --else
        -- If grip is not active, reset the count
        --count = 0
    --end
end


function detectNearestObject()
    local nearestH = -1
    local nearestD = 1000
    for i = 1, #handles, 1 do
        local vect = __getObjectPosition__(handles[i], parent)
        local D = math.sqrt(vect[1]^2 + vect[2]^2 + vect[3]^2)
        if (D<nearestD) and (D<maxPickDist) then
            nearestD = D
            nearestH = handles[i]
        end
    end
    return nearestH
end



function detectParent()
    self = sim.getObjectHandle(sim.handle_self)
    parent = sim.getObjectParent(self)
    active = false
    if parent==-1 then
        return
    end
    l = sim.getObjectHandle('LeftController')
    r = sim.getObjectHandle('RightController')
    if parent==l then
        mode = 1
        active = true
        __setObjectPosition__(self, l, {0,0,0})
        prefix = 'L_'
    elseif parent==r then
        mode = 2
        active = true
        __setObjectPosition__(self, r, {0,0,0})
        prefix = 'R_'
    end
end

-- All available integer signals
-- 'L_Trigger_Press'
-- 'R_Trigger_Press'
-- 'L_Trigger_Touch'
-- 'R_Trigger_Touch'
-- 'L_Grip_Press'
-- 'R_Grip_Press'
-- 'L_Grip_Touch'
-- 'R_Grip_Touch'
-- 'L_TrackPad_Press'
-- 'R_TrackPad_Press'
-- 'L_TrackPad_Touch'
-- 'R_TrackPad_Touch'
-- 'L_Joystick_Press'
-- 'R_Joystick_Press'
-- 'L_Joystick_Touch'
-- 'R_Joystick_Touch'
-- 'L_ApplicationMenu_Press'
-- 'R_ApplicationMenu_Press'
-- 'L_ApplicationMenu_Touch'
-- 'R_ApplicationMenu_Touch'

-- All available float signals
-- 'L_Trackpad_pos_x'
-- 'L_Trackpad_pos_y'
-- 'L_Trackpad_pos_z' -> I don't think this actually is something, but who knows
-- 'R_Trackpad_pos_x'
-- 'R_Trackpad_pos_y'
-- 'R_Trackpad_pos_z' -> I don't think this actually is something, but who knows
-- 'L_Joystick_pos_x' -> joystick, whatever this might be
-- 'L_Joystick_pos_y'
-- 'L_Joystick_pos_z'
-- 'R_Joystick_pos_x'
-- 'R_Joystick_pos_y'
-- 'R_Joystick_pos_z'