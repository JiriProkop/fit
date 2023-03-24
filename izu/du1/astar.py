from util import Map, Cell, Pos, WALL


class AStar:
    flag = False

    def __init__(self, _map: Map, start: Cell, goal: Cell, _open: list[Cell], closed: list[Cell]) -> None:
        self.map = _map
        self.start = start
        self.goal = goal
        self.open = _open
        self.closed = closed

        self.current = self.start

    def step(self) -> None:
        if not self.flag:
            self.current = self.open[0]
        self.flag = False
        for cell in self.open:
            if self.current.f > cell.f:
                self.current = cell

        self.open.remove(self.current)
        self.closed.append(self.current)
        if self.current == self.goal:
            return


        for nei in self.map.neighbours_of(self.current):
            if nei.cost == 'Z' or nei in self.closed:
                continue
            gscore = self.current.g + nei.cost
            if gscore < nei.g or nei not in self.open:
                nei.g = gscore
                nei.parent = self.current
                if nei not in self.open:
                    self.open.append(nei)

        # Write your code here
        # See https://cs.wikipedia.org/wiki/A*#Pseudok%C3%B3d
        # Implement body of the main loop
        # Set self.current to currently checked cell before returning

    def reconstruct_path(self) -> list[Cell]:
        lst = []
        curr = self.goal
        while curr is not None:
            lst.insert(0, curr)
            curr = curr.parent
        return lst
        # Write your code here
        # Return list of cells
        # First element should be self.start
        # Last element should be self.goal
