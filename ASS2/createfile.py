# create a file named inputFile.txt that consists of a name_surname with a num in the end, space and a vote

import random

def createFile():
	file = open("inputFile.txt", "w")
	for i in range(120):
		file.write("voter_name" + str(i) + " " + "party_name" + str(random.randint(1, 5)) + "\n")
	file.close()
    
createFile()