image_file = open("ascii_image.txt")

lines = image_file.readlines()
for line in lines:
    print(line, end='')