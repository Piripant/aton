SCRIPT_TYPE = "GRID"
TAGS = {
    {0, "Empty"},
    {1, "Strape"}
}

x = 0
y = 0

function world_step ()
    grids.set_current(x, y, 1)
    x = (x + 1) % grids.get_width()
    y = (y + 1) % grids.get_height()
end