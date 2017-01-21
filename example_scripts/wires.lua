NEIGHBORS_POS = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} }
STATES_NUM = 4
COLORS = { {255, 255, 255}, {20, 20, 20}, {20, 20, 20}, {255, 255, 0} }

empty = 0
wire = 1
disabled_wire = 2
signal = 3

function get_next_state (neighbors, state)
    if state == empty then
        return empty
    elseif state == signal then
        return disabled_wire
    elseif state == disabled_wire then
        return disabled_wire
    end

    for i, neighbor in pairs(neighbors) do
        if neighbor == signal then
            return signal
        end
    end

    return wire
end