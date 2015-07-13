# based on Adventure 6, Adventures in Minecraft
# csvBuildPattern.py

# Developed for the Minecraft Punchcard Reader
# by: Gemma May Latham and David Whale

import mcpi.minecraft as minecraft
import mcpi.block as block

mc = minecraft.Minecraft.create()

GAP = block.AIR.id
WALL = block.BOOKSHELF.id
FILENAME = "card.csv"
f = open(FILENAME, "r")

pos = mc.player.getTilePos()
ORIGIN_X = pos.x+1
ORIGIN_Y = pos.y
ORIGIN_Z = pos.z+1

z = ORIGIN_Z

while True:
    for line in f.readlines():
        line = line.strip()
        data = line.split(",")
        y = ORIGIN_Y

        for cell in data:
            if cell == "0":
                b = GAP
            else:
                b = WALL
            mc.setBlock(ORIGIN_X, y, z, b)
            y = y + 1
        z = z + 1

    raw_input("press enter to build again...")
          

# END

