from math import sqrt, inf
from dataclasses import dataclass

WALL = 'Z'

@dataclass
class Pos:
    x: int
    y: int

    @property
    def neighbours(self) -> list['Pos']:
        return [
            Pos(self.x - 1, self.y - 1),
            Pos(self.x, self.y - 1),
            Pos(self.x + 1, self.y - 1),
            Pos(self.x - 1, self.y),
            Pos(self.x + 1, self.y),
            Pos(self.x - 1, self.y + 1),
            Pos(self.x, self.y + 1),
            Pos(self.x + 1, self.y + 1)
        ]

    def __eq__(self, __o: object) -> bool:
        return isinstance(__o, Pos) and self.x == __o.x and self.y == __o.y

    def __ne__(self, __o: object) -> bool:
        return not self.__eq__(__o)

    def __str__(self) -> str:
        return f'[{self.x}, {self.y}]'

    @staticmethod
    def distance(pos1: 'Pos', pos2: 'Pos') -> float:
        return sqrt((pos1.x - pos2.x) ** 2 + (pos1.y - pos2.y) ** 2)

class Cell:
    def __init__(self, position: Pos, parent: 'Cell|None', cost: 'int|str', g: float, h: float) -> None:
        self.position = position
        self.parent = parent
        self.cost = cost
        self.__g = g
        self.__h = h

    @property
    def g(self):
        return self.__g

    @g.setter
    def g(self, value):
        self.__g = round(value, 2)

    @property
    def h(self):
        return self.__h
    
    @h.setter
    def h(self, value):
        self.__h = round(value, 2)

    @property
    def f(self):
        return round(self.__g + self.__h, 2)

    def __str__(self) -> str:
        return f'{self.position}, {self.f}, {"NULL" if self.parent is None else self.parent.position}'
    
    def __eq__(self, other):
        return self.position == other.position

class Map:
    def __init__(self, data: list[list[str]]) -> None:
        for y, row, in enumerate(data):
            for x, cell in enumerate(row):
                if cell.isdigit():
                    cost = int(cell)
                else:
                    cost = WALL
                data[y][x] = Cell(Pos(x, y), None, cost, inf, inf)
        self.__data: list[list[Cell]] = data

    @property
    def width(self):
        return len(self.__data[0])

    @property
    def height(self):
        return len(self.__data)

    def __getitem__(self, indices: 'tuple[int,int]|Pos') -> 'Cell|None':
        try:
            if isinstance(indices, Pos):
                return self.__data[indices.y][indices.x]
            if isinstance(indices, tuple) and len(indices) == 2 and isinstance(indices[0], int) and isinstance(indices[1], int):
                return self.__data[indices[1]][indices[0]]
        except IndexError:
            return None
        raise ValueError()

    def neighbours_of(self, cell: Cell) -> list[Cell]:
        return [self[n] for n in cell.position.neighbours if self[n] is not None]

    def calculate_heuristic(self, goal: Pos):
        for row in self.__data:
            for cell in row:
                cell.h = Pos.distance(cell.position, goal)
