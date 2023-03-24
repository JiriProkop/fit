from glob import glob
from pypdf import PdfReader, PdfWriter
from pypdf.generic import NameObject, TextStringObject, DictionaryObject
import re
from util import Map, Pos, Cell
from astar import AStar

source_files = glob('x[a-z][a-z][a-z][a-z][a-z][0-9a-z][0-9a-z].pdf')
if not source_files:
    print('Missing source file (name formatted as xlogin00.pdf).')
    exit(1)
if len(source_files) > 1:
    print(f'Too many matching file names ({", ".join(source_files[:3])}...)')
    exit(1)

reader = PdfReader(source_files[0])
first_page = reader.pages[0]
first_text = first_page.extract_text()

goal_regex = r': \(\[(\d+), (\d+)\], X, \[\?, \?\]\)'
matches = re.search(goal_regex, first_text, re.MULTILINE)
if not matches:
    print('Goal not found.')
    exit(1)
goal = [int(g) for g in matches.groups()]

table_regex = r'y\/x0123456789\r?\n\d(.*)\r?\n\d(.*)\r?\n\d(.*)\r?\n\d(.*)\r?\n\d(.*)\r?\n\d(.*)\r?\n\d(.*)\r?\n\d(.*)\r?\n\d(.*)\r?\n\d(.*)'
matches = re.search(table_regex, first_text, re.MULTILINE)
if not matches:
    print('Table not found.')
    exit(1)
table = [list(g) for g in matches.groups()]

def table_write(cell: DictionaryObject, value: str) -> None:
    cell.update({ NameObject('/V'): TextStringObject(value) })

def take(source: list, count: int) -> tuple[list, list]:
    return source[:count], source[count:]

def format_float(f: float) -> str:
    s = str(round(f, 2))
    if '.' not in s:
        s += '.0'
    elif not s.endswith('.0') and s[::-1].find('.') < 2:
        s += '0'
    return s

annotations = [annot.get_object() for page in reader.pages for annot in page.annotations]

result_annotations, annotations = take(annotations, 20)
help_annotations, annotations = take(annotations, 120)
iterations = []
while len(annotations) > 0:
    table_open, annotations = take(annotations, 20)
    table_closed, annotations = take(annotations, 20)
    iterations.append((table_open, table_closed))

reshuffled_help = []
for i in range(0, len(help_annotations), 8):
    reshuffled_help.append(help_annotations[i:i+4])
for i in range(4, len(help_annotations), 8):
    reshuffled_help.append(help_annotations[i:i+4])
help_annotations = reshuffled_help

try:
    first_row = help_annotations[0]
    starting_point = [int(s) for s in first_row[0]['/V'][1:-1].split(', ')]
    starting_g = float(first_row[1]['/V'])
    starting_h = float(first_row[2]['/V'])
    starting_f = float(first_row[3]['/V'])
except:
    print('Starting data failed to parse.')
    exit(1)

for row in table:
    print(' '.join(row))
print(f'{goal=}')
print(f'{starting_point=} {starting_g=} {starting_h=} {starting_f=}')

start_pos = Pos(starting_point[0], starting_point[1])
goal_pos = Pos(goal[0], goal[1])
_map = Map(table)
_map.calculate_heuristic(goal_pos)
_map[start_pos].g = starting_g
astar = AStar(_map, _map[start_pos], _map[goal_pos], [_map[start_pos]], [])
for iteration in iterations[1:]:
    astar.step()
    current_cell = _map[astar.current.position]
    table_write(help_annotations[0][0], str(current_cell.position))
    table_write(help_annotations[0][1], str(int(current_cell.g)))
    table_write(help_annotations[0][2], format_float(current_cell.h))
    table_write(help_annotations[0][3], format_float(current_cell.f))
    help_annotations = help_annotations[1:]
    if not astar.open or astar.current == astar.goal:
        break
    for pos, cell in zip(astar.open, iteration[0]):
        table_write(cell, str(pos))
    for pos, cell in zip(astar.closed, iteration[1]):
        table_write(cell, str(pos))

path = astar.reconstruct_path()
if not isinstance(path, list) or any([not isinstance(c, Cell) for c in path]):
    print('A* returned invalid path')
    exit(1)

if len(path) > len(result_annotations):
    print(f'Warning: whole path doesn\'t fit the table ({len(path)} > {len(result_annotations)})')
    path = path[:len(result_annotations)]

for pos, cell in zip(path, result_annotations):
    table_write(cell, str(pos.position))

writer = PdfWriter()
writer.append(reader)
with open('result.pdf', 'wb') as fp:
    writer.write(fp)
