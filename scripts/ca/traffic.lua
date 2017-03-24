SCRIPT_TYPE = "CA"

NEIGHBORS_POS = { {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {-1, -1} }
STATES_NUM = 3

right = 1
left = 2
up = 3
down = 4
down_left = 5

empty = 0
red_car = 1
blue_car = 2

function get_next_state (neighbors, state)
    if state == red_car and neighbors[right] == empty then
        return empty
    end
    
    if state == blue_car and neighbors[down] == empty and neighbors[down_left] ~= red_car then
        return empty
    end

    if state == empty then
        if neighbors[left] == red_car then
            return red_car
        elseif neighbors[up] == blue_car then
            return blue_car
        end
    end

    return state
end