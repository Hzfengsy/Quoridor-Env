import json

def load(filename):
	with open(filename) as json_file:
		data = json.load(json_file)
	return data

if __name__ == "__main__":
	json_data = load("result.json")
	with open('record.txt', 'w') as f:
		f.write("Init\n")
		steps = json_data['step']
		for x in steps:
			f.write(str(x['kind']) + ' ' + str(x['x']) + ' ' + str(x['y']) + '\n')
		