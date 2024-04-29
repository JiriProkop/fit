def read_char_table():
    char_table = [
        [None, 'V', 'W', 'X', 'Y', 'Z', 'Ž', '.', '?', '-'],
        ['/', '1', '2', '3', '4', '5', '6', '7', '8', '9'],
        ['0', 'A', 'B', 'C', 'Č', 'D', 'E', 'Ě', 'F', 'G'],
        ['H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q'],
        ['R', 'Ř', 'S', 'Š', 'T', 'U', None, None, None, None],
    ]
    return char_table

def translate_numbers_to_chars(numbers, char_table):
    res = []
    translated_chars = ''
    for line in numbers.split('\n'):
        if line.strip():
            numbers_list = line.split()
            for num in numbers_list:
                first_digit = int(num[0])
                second_digit = int(num[1])
                translated_chars += char_table[first_digit][second_digit]
            res.append(translated_chars)
            translated_chars = ''
    return res


def main():
    with open('cipher_forauto.txt', 'r') as file:
        numbers = file.read()

    char_table = read_char_table()
    translated_chars = translate_numbers_to_chars(numbers, char_table)

    print("Translated characters:")
    for line in translated_chars:
        if 'P' in line and 'R' in line and 'O' in line and 'K' in line:
            print(line)


if __name__ == "__main__":
    main()
