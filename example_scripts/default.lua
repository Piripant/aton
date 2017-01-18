NEIGHBORS_POS = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} }
STATES_NUM = 2
COLORS = { {255, 255, 255}, {127, 127, 127}, {0, 0, 0} }

function get_next_state (neighbors, state)
    if state == 1 then
        return 0
    end

    for i, neighbor in pairs(neighbors) do
        if neighbor == 1 then
            return 1
        end
    end

    return 0
end