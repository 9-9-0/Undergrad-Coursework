#!/usr/bin/python3
import tkinter
import csv
import subprocess
import shlex

top = tkinter.Tk()
current_record = 1
temp_new_record = []

decisionTreeExec = "../decisionTree/decisionTree"
neuralNetExec = "../neuralNetwork/neural-network"

weightsFileName = "../example-data/EachLetter100-Bool-a-weights2.csv"
#inputFileName = "../example-data/EachLetter100-Bool-a.csv"
inputFileName = "../example-data/bigDataCleaned.csv"
test_file_name = "../example-data/testOutput.csv"
top.checkboxVariable = tkinter.IntVar()
top.textboxVariable = tkinter.StringVar()
top.labelnnVariable = tkinter.StringVar()
top.labeldtVariable = tkinter.StringVar()
top.buttonDown = False

def goToRecord(cur):
	if cur < 1:
		cur = 1
	elif cur >= len(handwriting_csv):
		cur = len(handwriting_csv) - 1
	for i in range(128):
		if handwriting_csv[cur][i] == '1':
			C[i]["background"] = "black"
		else:
			C[i]["background"] = "white"
	top.checkboxVariable.set(int(handwriting_csv[cur][-1]))
	top.textboxVariable.set(str(cur))
	return cur
	
def goToTextBox():
	global current_record
	try:
		value = int(top.textboxVariable.get())
		if value < len(handwriting_csv):
			current_record = value
		else:
			current_record = len(handwriting_csv) - 1
		current_record = goToRecord(current_record)
	except:
		print("Invalid number",top.textboxVariable.get())

def goToNext():
	global current_record
	current_record = current_record + 1
	current_record = goToRecord(current_record)
	testRecord()

def goToPrev():
	global current_record
	if current_record > 1:
		current_record = current_record - 1
		current_record = goToRecord(current_record)
		testRecord()

def createNew():
	for i in range(128):
		C[i]["background"] = "white"

def testRecord():
	global temp_new_record
	global test_file_name
	temp_new_record = []
	for i in range(128):
		if C[i]["background"] == "white":
			temp_new_record.append(0)
		else:
			temp_new_record.append(1)
	control_row = [128,1,1]
	temp_new_record.append(int(top.checkboxVariable.get()))
	with open(test_file_name, 'w') as f:
		writer = csv.writer(f)
		writer.writerow(control_row)
		writer.writerow(temp_new_record)
	nncmd = neuralNetExec + ' -l 1 -n 10 -f ' + test_file_name + ' -w ' + weightsFileName + ' -m 2'
	p1 = subprocess.Popen(shlex.split(nncmd),stdout=subprocess.PIPE)
	nnOut = p1.stdout.read().decode('ascii').strip()
	top.labelnnVariable.set(nnOut)
	
	dtcmd = decisionTreeExec + ' ' + ' '.join(str(x) for x in temp_new_record)
	p2 = subprocess.Popen(shlex.split(dtcmd),stdout=subprocess.PIPE)
	dtOut = p2.stdout.read().decode('ascii').strip()
	top.labeldtVariable.set(dtOut)

def unclick(event):
	top.buttonDown = False

def click(event):
	top.buttonDown = True
	back_colr = event.widget["background"]
	if (back_colr == "white"):
		event.widget["background"] = "black"
	else:
		event.widget["background"] = "white"

def entered(event):
	if top.buttonDown:
		back_colr = event.widget["background"]
		if (back_colr == "white"):
			event.widget["background"] = "black"
		else:
			event.widget["background"] = "white"

with open(inputFileName,'r') as f:
	reader = csv.reader(f)
	handwriting_csv = list(reader)

leftFrame = tkinter.Frame(top)
leftFrame.pack(side=tkinter.LEFT)

textBoxFrame = tkinter.Frame(leftFrame)
textBoxFrame.pack(pady=10)

textBoxLabel = tkinter.Label(textBoxFrame, text="Go to record:")
textBoxLabel.pack(side = tkinter.LEFT)

textBox = tkinter.Entry(textBoxFrame, width = 6, textvariable = top.textboxVariable)
textBox.pack(side = tkinter.LEFT)

textBoxReadButton = tkinter.Button(textBoxFrame, text="go", command = goToTextBox)
textBoxReadButton.pack(side=tkinter.LEFT)

C = []
frames = []
top.bind("Button-1", click)
top.bind("ButtonRelease-1", unclick)
for i in range(16):
	frames.append(tkinter.Frame(leftFrame))
	frames[i].pack()
	for j in range(8):
		C.append(tkinter.Canvas(frames[i], bg="white", height=25,width=25))
		C[i * 8 + j].pack(side=tkinter.LEFT)
		C[i*8 + j].bind("<Button-1>",click)
		C[i*8 + j].bind("<ButtonRelease-1>",unclick)
		#C[i*8 + j].bind("<Enter>",entered)

buttonFrame = tkinter.Frame(leftFrame)
buttonFrame.pack(pady=10, padx=10)

createNewButton = tkinter.Button(buttonFrame, text="new", command = createNew)
createNewButton.pack(side=tkinter.LEFT)

prevButton = tkinter.Button(buttonFrame, text="prev", command = goToPrev)
prevButton.pack(side=tkinter.LEFT)

nextButton = tkinter.Button(buttonFrame, text="next", command = goToNext)
nextButton.pack(side=tkinter.LEFT)

testButton = tkinter.Button(buttonFrame, text="test", command = testRecord)
testButton.pack(side=tkinter.LEFT)

positiveCheckbox = tkinter.Checkbutton(buttonFrame, text="correct", onvalue = 1, offvalue = 0, variable = top.checkboxVariable)
positiveCheckbox.pack(side=tkinter.LEFT)

#labels that will be updated when programs run
rightFrame = tkinter.Frame(top)
rightFrame.pack(side=tkinter.LEFT,padx=(10,30))

labelFrame = tkinter.Frame(rightFrame)
labelFrame.pack(side=tkinter.RIGHT)

tkinter.Label(labelFrame, text = "Neural net prediction").pack()
nnLabel = tkinter.Label(labelFrame, textvariable = top.labelnnVariable)
nnLabel.pack()

tkinter.Label(labelFrame, text = "Decision tree prediction").pack()
dtLabel = tkinter.Label(labelFrame, textvariable = top.labeldtVariable)
dtLabel.pack()

goToRecord(1)

top.mainloop()
