# create a file named inputFile.txt that consists of a name_surname with a num in the end, space and a vote

import random

def createFile1():
	file = open("inputFile.txt", "w")
	for i in range(120):
		file.write("voter_name" + str(i) + " " + "party_name" + str(random.randint(1, 5)) + "\n")
	file.close()

def createFile2():
	file = open("politicalParties.txt", "w")
	for i in range(10):
		file.write("party_name" + str(i)+ "\n")
	file.close()
    
# createFile()
createFile2()