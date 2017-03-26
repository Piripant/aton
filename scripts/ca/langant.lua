SCRIPT_TYPE = "CA"
TAGS = { 
    {0, "Empty"}, 
    {1, "Full"},
    {2, "Empty up ant"},
    {3, "Empty right ant"},
    {4, "Empty down ant"},
    {5, "Empty left ant"},
    {6, "Full up ant"},
    {7, "Full right ant"},
    {8, "Full down ant"},
    {9, "Full left ant"} 
}

NEIGHBORS_POS = { {0, 1}, {1, 0}, {0, -1}, {-1, 0} }
STATES_NUM = 10

-- States
empty = 0
full = 1

empty_up_ant = 2
empty_right_ant = 3
empty_down_ant = 4
empty_left_ant = 5

full_up_ant = 6
full_right_ant = 7
full_down_ant = 8
full_left_ant = 9

-- Rotations. To convert a rotation to a neighbor position in the neighbors array just add 1
up = 0
right = 1
down = 2
left = 3

function get_next_state (neighbors, state)
    -- If the ant was over this block flip it's value
    if block_under_ant(state) == empty then
        return full
    elseif block_under_ant(state) == full then
        return empty
    end

    -- The rotation of the ant on each block. If there is no ant the value will be -1
    local up_neigh = turn(neighbors[up + 1])
    local right_neigh = turn(neighbors[right + 1])
    local down_neigh = turn(neighbors[down + 1])
    local left_neigh = turn(neighbors[left + 1])

    -- The ant is heading this way
    if get_ant_rotation(up_neigh) == down then
        return set_ant(state, up_neigh)
    
    elseif get_ant_rotation(right_neigh) == left then
        return set_ant(state, right_neigh)
    
    elseif get_ant_rotation(down_neigh) == up then
        return set_ant(state, down_neigh)
    
    elseif get_ant_rotation(left_neigh) == right then
        return set_ant(state, left_neigh)
    end

    return state
end

function set_ant(block_state, ant_state)
    if block_state == empty then
        return 2 + get_ant_rotation(ant_state)
    else
        return 6 + get_ant_rotation(ant_state)
    end
end

function get_ant_rotation(state)
    if block_under_ant(state) == empty then
        return state - 2
    elseif block_under_ant(state) == full then
        return state - 6
    else
        return -1 -- there is no ant on this block
    end
end

function block_under_ant(state)
    if state >= 2 and state <= 5 then
        return empty
    elseif state >= 6 and state <= 9 then
        return full
    else
        return -1 -- there is no ant on the block
    end
end

function turn (state)
    if block_under_ant(state) == empty then -- if it empty under
        return (state + 1 + 2) % 4 + 2
    elseif block_under_ant(state) == full then
        return (state - 1 + 6) % 4 + 6
    else
        return state
    end
end