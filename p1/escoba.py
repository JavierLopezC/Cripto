import re
import sys
import unidecode

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Error: no hay suficientes argumentos\nescoba.py inFile outFile")
        sys.exit(2)
    inFile = sys.argv[1]
    outFile = sys.argv[2]
    with open(inFile, 'r') as f:
        text = f.read()
    text = unidecode.unidecode(text)
    text = text.upper()
    text = re.sub(r'[^A-Z]', '', text)
    with open(outFile, 'w') as f:
        text = f.write(text)
    sys.exit(0)
