#!/opt/local/bin/python

import csv
import sys

from pathlib import Path
from PIL     import Image

DEBUG        = False

verifyExpectedSize = False
verifyScaledSize   = False

# hardwired defaults
ATLAS_WIDTH  = 1024 
ATLAS_HEIGHT = 2048
IMAGE_WIDTH  = 16

csvfilename  = "BA1A_fullscale_sizes.csv"
outfilename  = "BA1A_downsascaled_atlas.png"

filterbymineral = True
chosenmineral   = "3"

baseFolder      = "_BA1A/fullscale"


def readCSV(csvfilename):
    # Read the CSV, preserving identifiers such as SECTION and MINERAL.
    with open(csvfilename, "r", newline="", encoding="utf-8-sig") as csv_file:
        return list(csv.DictReader(csv_file))


def filterRows(rows, mineral):
    # Keep only rows matching the selected MINERAL value.
    if mineral.lower() != 'hydration':
        n_m = int(mineral.strip())
        return [row for row in rows if int(row["MINERAL"].strip()) == n_m]
    return [row for row in rows if row["MINERAL"].strip().lower() == 'hydration']


def createAtlas():
    # Create a blank 2048x2048, 8-bit grayscale image.
    return Image.new("L", (ATLAS_WIDTH, ATLAS_HEIGHT), color=0)


def readScaleAppend(atlas, row, imagefullname, x, y):
    # Read, validate, resize, and place one image in the atlas.
    # Returns the resized image height, or None if the image cannot be read
    # or its dimensions do not match the CSV.

    try:
        with Image.open(imagefullname) as source:
            source.load()

            # Validate the original image against the CSV.
            if verifyExpectedSize:
                expected_width = int(row["WIDTH"])
                expected_height = int(row["HEIGHT"])

                if source.size != (expected_width, expected_height):
                    print(
                        f"Skipping {imagefullname}: expected "
                        f"{expected_width}x{expected_height}, "
                        f"found {source.width}x{source.height}"
                    )
                    return None

            # Convert to 8-bit grayscale.
            grayscale=source
            if source.mode != "L":
              grayscale = source.convert("L")

            # Resize proportionally, rounding the height down.
            new_height = int(source.height * IMAGE_WIDTH / source.width)

            if new_height < 1:
                print(f"Skipping {imagefullname}: resized height is too small")
                return None

            resized = grayscale.resize(
                (IMAGE_WIDTH, new_height),
                Image.Resampling.LANCZOS,
            )

            if verifyScaledSize:
                if IMAGE_WIDTH == 16:
                    expected_scaled = resized.size
                    # Check the optional scaled dimensions recorded in the CSV.
                    if IMAGE_WIDTH==16 and "WIDTH16" in row and "HEIGHT16" in row:
                        expected_scaled = ( int(row["WIDTH16"]), int(row["HEIGHT16"]))
                    elif IMAGE_WIDTH==3 and "WIDTH3" in row and "HEIGHT3" in row:
                        expected_scaled = ( int(row["WIDTH3"]), int(row["HEIGHT3"]))
                    if resized.size != expected_scaled:
                        print(
                            f"Warning: {row['IMAGE']} resized to "
                            f"{resized.width}x{resized.height}; "
                            f"CSV specifies {expected_scaled[0]}x"
                            f"{expected_scaled[1]}"
                        )

            # Place the resized image at its assigned position.
            atlas.paste(resized, (x, y))

            return new_height

    except (OSError, ValueError) as error:
        print(f"Skipping {imagefullname}: {error}")
        return None


def buildAtlas(rows):
    # Pack images top-to-bottom in columns, moving left-to-right.
    atlas = createAtlas()

    x = 0
    y = 0
    images_placed = 0

    for row in rows:
        # Calculate the proportional height before deciding placement.
        try:
            source_width = int(row["WIDTH"])
            source_height = int(row["HEIGHT"])
            image_width = IMAGE_WIDTH
            image_height = int(source_height * IMAGE_WIDTH / source_width)
        except (ValueError, ZeroDivisionError):
            print(f"Skipping invalid CSV dimensions: {row['IMAGE']}")
            continue

        if image_height < 1 or image_height > ATLAS_HEIGHT:
            print(f"Skipping image too tall for atlas: {row['IMAGE']}")
            continue

        # Start a new column if this image will not fit vertically.
        if y + image_height > ATLAS_HEIGHT:
            x += IMAGE_WIDTH
            y = 0

        # Stop when the next column would exceed the atlas width.
        if x + IMAGE_WIDTH > ATLAS_WIDTH:
            print("Atlas is full; remaining images were not placed.")
            break

        imagefullname = f"{baseFolder}/{row['SECTION']}/{row['PIECE']}/{row['MINERAL']}/{row['IMAGE']}"
        if DEBUG:
            print(f"+ Placing {imagefullname} at ({x}, {y})")

        actual_height = readScaleAppend(atlas, row, imagefullname, x, y)

        # Advance only if the image was successfully placed.
        if actual_height is not None:
            y += actual_height
            images_placed = images_placed + 1

    if DEBUG:
        print(f"] Images selected: {len(rows)}")
        print(f"] Images placed:   {images_placed}")
        print(f"] Images omitted:  {len(rows) - images_placed}")

    return atlas


def exportAtlas(atlas, output_path):
    # Save the atlas as an 8-bit grayscale PNG.
    output_path = Path(output_path)
    output_path.parent.mkdir(parents=True, exist_ok=True)
    atlas.save(output_path, format="PNG")
    if DEBUG:
      print(f"> Atlas for {chosenmineral} saved to {output_path}")


def main(args):
    global DEBUG
    global outfilename
    global csvfilename
    global baseFolder
    global chosenmineral
    global ATLAS_WIDTH
    global ATLAS_HEIGHT
    global IMAGE_WIDTH
    global verifyExpectedSize
    global verifyScaledSize

    i = 1
    while i < len(args):
        if args[i] in ("-h", "--help"):
            print(
                "usage: compactFullscaletoSingle.py "
                "-csv <csv_file> -folder <base_folder> "
                "-mineral <MINERAL> [-o <output_file>] "
                "[-verifyoriginalsize] [-verifyscaledsize]"
                "[-width <atlas_width>] [-height <atlas_height>] [-tilewidth <tile_width>]"
            )
            return
        if args[i] in ("-D", "-debug"):
            DEBUG = True
        elif args[i] in ("-o", "--output"):
            i = i + 1
            outfilename = args[i]
        elif args[i] == "-csv":
            i = i + 1
            csvfilename = args[i]
        elif args[i] == "-folder":
            i = i + 1
            baseFolder = args[i]
        elif args[i] == "-mineral":
            i = i + 1
            chosenmineral = args[i]
        elif args[i] == "-width":
            i = i + 1
            ATLAS_WIDTH = int(args[i])
        elif args[i] == "-tilewidth":
            i = i + 1
            IMAGE_WIDTH = int(args[i])
        elif args[i] == "-height":
            i = i + 1
            ATLAS_HEIGHT = int(args[i])
        elif args[i] == "-verifyoriginalsize":
            verifyExpectedSize = True
        elif args[i] == "-verifyscaledsize":
            verifyScaledSize = True
        i += 1

    rows = readCSV(csvfilename)

    if DEBUG:
        print(f"< Read {len(rows)} rows from {csvfilename}")

    if filterbymineral:
        rows = filterRows(rows, chosenmineral)
        if DEBUG:
            print(f": Filtered to {len(rows)} rows for MINERAL={chosenmineral}")

    if not rows:
        print(f"! No rows found for MINERAL={chosenmineral}")
        return

    atlas = buildAtlas(rows)
    exportAtlas(atlas, outfilename)
    return


if __name__ == "__main__":
    main(sys.argv)
