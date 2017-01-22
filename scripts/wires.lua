NEIGHBORS_POS = { {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {-1, -1}, {-1, 1}, {1, -1} }
STATES_NUM = 4

empty = 0
wire = 1
electron_head = 2
electron_tail = 3

function get_next_state (neighbors, state)
    if state == empty then
        return empty
    elseif state == electron_head then
        return electron_tail
    elseif state == electron_tail then
        return wire
    end

    local heads = 0
    for i, neighbor in pairs(neighbors) do
        if neighbor == electron_head then
            heads = heads + 1
        end
    end

    if heads == 1 or heads == 2 then
        return electron_head
    else
        return wire
    end
end